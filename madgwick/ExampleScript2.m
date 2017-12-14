% ExampleScript.m
%
% This script demonstrates use of the MadgwickAHRS and MahonyAHRS algorithm
% classes with example data. ExampleData.mat contains calibrated gyroscope,
% accelerometer and magnetometer data logged from an AHRS device (x-IMU)
% while it was sequentially rotated from 0 degrees, to +90 degree and then
% to -90 degrees around the X, Y and Z axis.  The script first plots the
% example sensor data, then processes the data through the algorithm and
% plots the output as Euler angles.
%
% Note that the Euler angle plot shows erratic behaviour in phi and psi
% when theta approaches �90 degrees. This due to a singularity in the Euler
% angle sequence known as 'Gimbal lock'.  This issue does not exist for a
% quaternion or rotation matrix representation.
%
% Date          Author          Notes
% 28/09/2011    SOH Madgwick    Initial release
% 13/04/2012    SOH Madgwick    deg2rad function no longer used
% 06/11/2012    Seb Madgwick    radian to degrees calculation corrected

%% Start of script

addpath('quaternion_library');      % include quaternion library
close all;                          % close all figures
clear;                              % clear all variables
clc;                                % clear the command terminal

%% Import and plot sensor data

%load('11_5flat_trial2.mat');
%load('11_5vertical_trial2.mat');
load('11_20benign2.mat');
load('11_20sophattack2.mat');
figure('Name', 'Sensor Data');
axis(1) = subplot(3,1,1);
hold on;
plot(time, gyroscope_calibrated(:,1), 'r');
plot(time, gyroscope_calibrated(:,2), 'g');
plot(time, gyroscope_calibrated(:,3), 'b');
legend('X', 'Y', 'Z');
xlabel('Time (s)');
ylabel('Angular rate (deg/s)');
title('Gyroscope');
hold off;
axis(2) = subplot(3,1,2);
hold on;
plot(time, accelerometer(:,1), 'r');
plot(time, accelerometer(:,2), 'g');
plot(time, accelerometer(:,3), 'b');
legend('X', 'Y', 'Z');
xlabel('Time (s)');
ylabel('Acceleration (g)');
title('Accelerometer');
hold off;
axis(3) = subplot(3,1,3);
hold on;
plot(time, magnetometer_calibrated(:,1), 'r');
plot(time, magnetometer_calibrated(:,2), 'g');
plot(time, magnetometer_calibrated(:,3), 'b');
legend('X', 'Y', 'Z');
xlabel('Time (s)');
ylabel('Flux (G)');
title('Magnetometer');
hold off;
linkaxes(axis, 'x');

%% Process sensor data through algorithm

AHRS = MadgwickAHRS('SamplePeriod', 1/256, 'Beta', 0.1);
% AHRS = MahonyAHRS('SamplePeriod', 1/256, 'Kp', 0.5);

quaternion = zeros(length(time), 4);
for t = 1:length(time)
    AHRS.Update(gyroscope_calibrated(t,:), accelerometer(t,:), magnetometer_calibrated(t,:));	% gyroscope units must be radians
    quaternion(t, :) = AHRS.Quaternion;
end
quaternion_1 = zeros(length(time_1), 4);
for t = 1:length(time_1)
    AHRS.Update(gyroscope_calibrated_1(t,:), accelerometer_1(t,:), magnetometer_calibrated_1(t,:));	% gyroscope units must be radians
    quaternion_1(t, :) = AHRS.Quaternion;
end
%% Plot algorithm output as Euler angles
% The first and third Euler angles in the sequence (phi and psi) become
% unreliable when the middle angles of the sequence (theta) approaches �90
% degrees. This problem commonly referred to as Gimbal Lock.
% See: http://en.wikipedia.org/wiki/Gimbal_lock

%mapping both different quaternions
%euler = quatern2euler(quaternConj(quaternion)) * (180/pi);	% use conjugate for sensor frame relative to Earth and convert to degrees.
euler=quaternion*(180/pi)
%euler_1 = quatern2euler(quaternConj(quaternion_1)) * (180/pi);	% use conjugate for sensor frame relative to Earth and convert to degrees.
euler_1=quaternion_1*(180/pi)

figure('Name', 'Euler Angles before orientation');
axis(1) = subplot(3,1,1);
hold on;
plot(time, euler(:,2));
plot(time_1, euler_1(:,2));
xlabel('Time (s)');
ylabel('roll');
%legend('attack', 'actual');
hold off;
axis(2) = subplot(3,1,2);
hold on;
plot(time, euler(:,3));
plot(time_1, euler_1(:,3));
xlabel('Time (s)');
ylabel('pitch');
legend('normal', 'attacker');
hold off;
axis(3) = subplot(3,1,3);
hold on;
plot(time, euler(:,4));
plot(time_1, euler_1(:,4));
%title('Euler angles');
xlabel('Time (s)');
ylabel('yaw');
%legend('horizontal', 'vertical');
hold off;



%% making second quaternion orient to first's frame of reference


if(length(quaternion)<length(quaternion_1))
    quaternion_1=quaternion_1(1:length(quaternion),:);
else
    if(length(quaternion)>length(quaternion_1))
        quaternion=quaternion(1:length(quaternion_1),:);
        time=time_1;
    end
end
relative_or=quaternion_1.*quaternion;
%quaternion_1=(quaternion.*inv);
quaternion_1=(quaternion_1.*relative_or);
%euler = quatern2euler(quaternConj(quaternion)) * (180/pi);	% use conjugate for sensor frame relative to Earth and convert to degrees.
euler=quaternion*(180/pi)
%euler_1 = quatern2euler(quaternConj(quaternion_1)) * (180/pi);	% use conjugate for sensor frame relative to Earth and convert to degrees.
euler_1=quaternion_1*(180/pi)

%mapping oriented quaternions
figure('Name', 'Euler Angles after orientation');
axis(1) = subplot(3,1,1);
hold on;
plot(time, euler(:,2));
plot(time, euler_1(:,2));
xlabel('Time (s)');
ylabel('roll');
%legend('horizontal', 'vertical');
hold off;
axis(2) = subplot(3,1,2);
hold on;
plot(time, euler(:,3));
plot(time, euler_1(:,3));
xlabel('Time (s)');
ylabel('pitch');
%legend('horizontal', 'vertical');
hold off;
axis(3) = subplot(3,1,3);
hold on;
plot(time, euler(:,4));
plot(time, euler_1(:,4));
%title('Euler angles');
xlabel('Time (s)');
ylabel('yaw)');
%legend('horizontal', 'vertical');
hold off;
%% End of script