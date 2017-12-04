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
import errno
from os import path
from os import makedirs


DEBUG = True


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
    assert pt == at and len(pt) == 1, \
        'powerdue and app data frames don have different times'
    pr, _ = powerdue.shape
    ar, _ = app.shape
    if pr == ar:
        # powerdue and app contain same number of readings
        app_no_time = app[app_col_names[1:]]
        combined = [powerdue, app_no_time]
        new_df = pandas.concat(combined, axis=1, ignore_index=True)
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
        assert not (len_last_rows & 0x1), \
            'last_rows shoul have an even number of rows'
        avg_rows = []
        for i in range(0, len_last_rows, 2):
            row1, row2 = last_rows[i], last_rows[i+1]
            # Get the time stamp
            new_row = [row1[0]]
            for a, b in zip(row1[1:], row2[1:]):
                new_row.append((a+b) / 2)
            # Append the new row
            avg_rows.append(new_row)
        first_rows.extend(avg_rows)
        assert len(first_rows) == pr, 'row numbers do not match'
        powerdue_rows = powerdue.values.tolist()
        new_rows = []
        for prow, arow in zip(powerdue_rows, first_rows):
            # Don't include time from arow
            prow.extend(arow[1:])
            new_rows.append(prow)
    else:
        # powerdue data frame contains more readings
        diff_rows = pr - ar
        app_rows = app.values.tolist()
        last_app_row = app_rows[-1]
        app_rows.extend([last_app_row] * diff_rows)
        assert len(app_rows) == pr, \
            'not repeating last row correct number of times'
        powerdue_rows = powerdue.values.tolist()
        new_rows = []
        for prow, arow in zip(powerdue_rows, app_rows):
            # Don't include time from arow
            prow.extend(arow[1:])
            new_rows.append(prow)
    return new_rows


# directory and file names
datadir = 'data'
outdir = 'clean'
powerdue_dir = 'powerdue'
ios_dir = 'ios'
expriment_dirs = ['exp1', 'exp2', 'exp3']
exp1_mappings = [
    # powerdue files, app files
    ('exp1_2.csv', 'exp1-1.csv'),
    ('exp1_3.csv', 'exp1-2.csv'),
    ('exp1_4.csv', 'exp1-3.csv'),
    ('exp1_5.csv', 'exp1-4.csv'),
    ('exp1_6.csv', 'exp1-5.csv'),
]
exp2_mappings = [
    # powerdue files, app files
    ('exp2_2.csv', 'exp2-2.csv'),
    ('exp2_3.csv', 'exp2-3.csv'),
    ('exp2_4.csv', 'exp2-4.csv'),
    ('exp2_5.csv', 'exp2-5.csv'),
    ('exp2_6.csv', 'exp2-6.csv'),
]
exp3_mappings = [
    # powerdue files, app files
    ('exp3_1.csv', 'exp3-1.csv'),
    ('exp3_2.csv', 'exp3-2.csv'),
    ('exp3_3.csv', 'exp3-3.csv'),
    ('exp3_4.csv', 'exp3-4.csv'),
    ('exp3_5.csv', 'exp3-5.csv'),
]
exp_mappings = {
    'exp1': exp1_mappings,
    'exp2': exp2_mappings,
    'exp3': exp3_mappings,
}

# Columns from PowerSense that we are interested in.
power_sense_cols = [0, 4, 5, 6, 10, 11, 12, 13, 14, 15]

# Names of columns for fields from PowerDue.
pd_col_names = [
    'unixtime',
    'rx', 'ry', 'rz',
    'ax', 'ay', 'az',
    'mx', 'my', 'mz'
]

# Names of columns for fields from app.
app_col_names = [
    'app_unixtime',
    'apprx', 'appry', 'apprz',
    'appax', 'appay', 'appaz',
    'appmx', 'appmy', 'appmz'
]

all_col_names = pd_col_names + app_col_names[1:]

no_output_file_msg = 'Unable to create new data file from %s and %s'


# Process data files
for exp in expriment_dirs:
    for exp_num, (pfname, afname) in enumerate(exp_mappings[exp], 1):

        powerdue_file = path.join(datadir, powerdue_dir, exp, pfname)
        app_file = path.join(datadir, ios_dir, exp, afname)
        if DEBUG:
            msg = 'Reading powerdue file %s and iOS app file %s'
            print(msg % (powerdue_file, app_file))

        # Read Powerdue data file
        pd_df = pandas.read_csv(powerdue_file)
        pd_df.columns = pd_col_names
        powerdue_seconds = pd_df.unixtime.unique()

        if DEBUG:
            msg = 'Created powerdue data frame with columns %s'
            print(msg % pd_df.columns)
            msg = 'The dimensions are %s'
            print(msg % str(pd_df.shape))

        # Read PowerSense data file
        app_df = pandas.read_csv(app_file, usecols=power_sense_cols)
        app_df.columns = app_col_names
        # Convert time as float to int
        app_df.app_unixtime = app_df.app_unixtime.astype(int)
        app_seconds = app_df.app_unixtime.unique()

        if DEBUG:
            msg = 'Created app data frame with columns %s'
            print(msg % app_df.columns)
            msg = 'The dimensions are %s'
            print(msg % str(app_df.shape))

        # Only care about readings from same time
        shared_seconds = set(powerdue_seconds) & set(app_seconds)
        pd_df = pd_df[pd_df.unixtime.isin(shared_seconds)]
        app_df = app_df[app_df.app_unixtime.isin(shared_seconds)]

        if DEBUG:
            msg = 'The data frames share %d secons'
            print(msg % len(shared_seconds))
            msg = 'Now dimensions are powerdue: %s app: %s'
            print(msg % (str(pd_df.shape), str(app_df.shape)))

        new_df_rows = []
        for sec in shared_seconds:
            pd_time_df = pd_df[pd_df.unixtime == sec]
            pd_rows, c = pd_time_df.shape


            app_time_df = app_df[app_df.app_unixtime == sec]
            app_rows, c = app_time_df.shape

            if DEBUG:
                msg = 'The shape of pd_time_df: %s'
                print(msg % str(pd_time_df.shape))
                msg = 'The shape of app_time_df: %s'
                print(msg % str(app_time_df.shape))

            # If we have less than 20 readings on a second then don't include
            if pd_rows < 30 or app_rows < 30:
                continue

            new_rows = normalize_rows(pd_time_df, app_time_df)
            if not new_rows:
                print('new_rows does not contain any rows')
            new_df_rows.extend(new_rows)

        if not new_df_rows:
            print(no_output_file_msg % (powerdue_file, app_file))
            continue

        # Create the output directory if it has not been created already
        output_path = path.join(datadir, outdir, exp)
        try:
            makedirs(output_path)
        except OSError as err:
            if err.errno != errno.EEXIST:
                raise

        output_file = path.join(output_path, '%s-%d.csv' % (exp, exp_num))

        # Create new data frame and save the data
        new_df = pandas.DataFrame(new_df_rows)
        new_df.columns = all_col_names
        new_df.unixtime = new_df.unixtime.astype(int)
        new_df.to_csv(output_file, index=False)
