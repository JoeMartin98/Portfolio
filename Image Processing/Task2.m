% MATLAB script for Assessment Item-1
% Task-2
clear; close all; clc;

% Step-1: Load input image
I = imread('Noisy.png');

% Step-2: Conversion of input image to grey-scale image
Igray = rgb2gray(I);

%creates a padded image, 2 pixels wide
paddedArray = padarray(Igray,[2 2]);

%get the size of both the gray image and the padded image
[m,n] = size(paddedArray);
[a,b] = size(Igray);

%creates the 2 new images, and an empty array
median = [];
medianImage = Igray;
meanImage = Igray;

%looks through every row in the gray image
for row = 1:a
    %looks through every column in the gray image
    for col = 1:b
        sum = 0;
        count = 1;
        %loops through the neighbouring rows
        for neighbourRow = -2:2
            %loops through the neighbouring columns
            for neighbourCol = -2:2
                %adds the neighbouring pixels together, converts it to a
                %double
                sum = sum + double(paddedArray(row+neighbourRow+2,col+neighbourCol+2));
                %puts the pixel values into an array
                median(count) = paddedArray(row+neighbourRow+2,col+neighbourCol+2);
                count = count +1;
            end
        end
        %sorts the array full of the pixel values
        median = sort(median);
        %sets the middle(median) value to be the value we pick for the
        %output
        medianImage(row,col) = median(13);
        %creates the average of the neighbouring pixels
        sum = sum/25;
        %sets the mean value to the be the value we pick
        meanImage(row,col) = sum;
    end
end

%output the images onto the same figure
figure;
subplot(1,3,1), imshow(Igray);
title("Gray image");
subplot(1,3,3), imshow(medianImage);
title("Median image");
subplot(1,3,2), imshow(meanImage);
title("Mean image");
