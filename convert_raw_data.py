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
