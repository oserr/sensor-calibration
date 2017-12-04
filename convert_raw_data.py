#!/usr/bin/env python
#
# File: convert_raw_data.py
# Author: Omar Serrano
# Date: 12/3/2017
#
# The purpose of the script is to convert the raw data from the PowerDue and
# PowerSense, the iOS app, into a form such that it is easier to analyze the
# data directly. There are some issues with the raw data that make it hard to
# analayze the data, including:
#   - The PowerSense data contains more samples per second.
#   - The readings for each experiment don't begin and end at the same time.
#   - The PowerSense data contains data fields that are not used.
# We resolve this issues by
#   - Only using readings with times that exist in the PowerDue data.
#   - In the case where the PowerSense app has more readings for a second, we
#     map some of readings into a single reading by taking the average of the
#     values.
#   - Dropping fields that are not used.
# In the end, we concatenate the matrix for the PowerDue data with the matrix
# for the PowerSense data, and create an output file with all of the exprimental
# data for a single experiment.

import pandas


def normalize_rows(powerdue, app):
    '''Creates a new set of data so that the number of rows in the powerdue
    data frame match the number of rows in the app data frame.

    :param powerdue
        A data frame representing the readings for a second from the PowerDue.
    :param app
        A data frame representing the readings for a second from PowerSense.
    :return
        A list of rows, where each row is a contenation of the values in
        powerdue and app. The data frames are normalized differently according
        to which data frame contains more readings:
        - same: the data frames are simply contatenated
        - powerdue has more: the last reading of app is repeated to bring the
          number of readings up to the number of readings in powerdue.
        - app has more: multiple readins in app are average into one reading to
          bring the number of readings down to the number of readings inthe
          powerdue data frame.
    '''
    pt = powerdue.unixtime.unique()
    at = app.app_unixtime.unique()
    assert pt == at and len(pt) == 1,
        'powerdue and app data frames don have different times'
    pr, _ = powerdue.shape
    ar, _ = app.shape
    if pr == ar:
        # powerdue and app contain same number of readings
        new_df = pandas.concat([powerdue, app], axis=1, ignore_index=True)
        new_rows = new_df.values.tolist()
    elif pr < ar:
        # powerdue data frame contains less readings
        diff_rows = ar - pr
        app_rows = app.values.tolist()
        # first_rows are kept as is
        first_rows = app_rows[:pr-diff_rows]
        # We take average of last_rows
        last_rows = app_rows[pr-diff_rows:]
        len_last_rows = len(last_rows)
        assert not (len_last_rows & 0x1),
            'last_rows shoul have an even number of rows'
        avg_rows = []
        for i in range(0, len_last_rows, 2):
            row1, row2 = last_rows[i], last_rows[i+1]
            # Get the time stamp
            new_row = [row1[0]]
            for a, b in zip(row1[1:], row2[1:]):
                new_row += (a+b) / 2
            # Append the new row
            avg_rows += new_row
        first_rows.extend(avg_rows)
        assert len(first_rows) == pr, 'row numbers do not match'
        powerdue_rows = powerdue.values.tolist()
        new_rows = []
        for prow, arow in zip(powerdue_rows, first_rows):
            prow.extend(arow)
            new_rows += prow
    else:
        # powerdue data frame contains more readings
        diff_rows = pr - ar
        app_rows = app.values.tolist()
        last_app_row = app_rows[-1]
        app_rows.extend([last_app_row] * diff_rows)
        assert len(app_rows) == pr,
            'not repeating last row correct number of times'
        powerdue_rows = powerdue.values.tolist()
        new_rows = []
        for prow, arow in zip(powerdue_rows, app_rows):
            prow.extend(arow)
            new_rows += prow
    return new_rows


# Columns from PowerSense that we are interested in.
power_sense_cols = [0, 4, 5, 6, 10, 11, 12, 13, 14, 15]

# Names of columns for fields from app.
app_col_names = [
    'app_unixtime',
    'app_rot_x', 'app_rot_y', 'app_rot_z',
    'app_acc_x', 'app_acc_y', 'app_acc_z',
    'app_mag_x', 'app_mag_y', 'app_mag_z'
]

# Read Powerdue data file
pd_file = 'powerdue_file.csv'
pd_df = pandas.read_csv(pd_file)
data_times = pd_df.unixtime.unique()

# Read PowerSense data file
app_file = 'app_file.csv'
app_df = pandas.read_csv(app_file, usecols=power_sense_cols)
app_df.columns = app_col_names
# Convert time as float to int
app_df.app_unixtime = app_df.app_unixtime.astype(int)
app_df = app_df[app_df.app_unixtime.isin(data_times)]

# For each time, find out how many readings we have, and then normalize by
# averaging the values of the data frame that has more readings.
new_df_rows = []
for sec in data_times:
    pd_time_df = pd_df[pd_df.unixtime == sec]
    pd_rows, _ = pd_time_df.shape

    app_time_df = app_df[app_df.app_unixtime == sec]
    app_rows, _ = app_time_df.shape

    # If we have less than 20 readings on a second then don't include
    if pd_rows < 30 or app_rows < 30:
        continue

    new_rows = normalize_rows(pd_time_df, app_time_df)
    new_df_rows.extend(new_rows)
