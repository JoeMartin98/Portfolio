% MATLAB script for Assessment Item-1
% Task-3
clear; close all; clc;

% Step-1: Load input image
I = imread('Starfish.jpg');

%converts the image to gray scaled
Igray = rgb2gray(I);
Igray = medfilt2(Igray, [5 5]);

%creates a binary image
BW = imbinarize(Igray, 0.9);
%inverts the image
BW = ~BW;

step1 = bwlabel(BW);
stats = regionprops(step1,'Area', 'Perimeter');
allBlobPerimeters = [stats.Perimeter];
allBlobAreas = [stats.Area];

allowablePerimeterIndexes = (allBlobPerimeters > 230) & (allBlobPerimeters < 320);
allowableAreaIndexes = allBlobAreas < 2000;
keeperIndexes = find(allowablePerimeterIndexes & allowableAreaIndexes);
keeperBlobsImage = ismember(step1, keeperIndexes);
numberOfBlobs = size(keeperBlobsImage, 1);

%displays the images in a figure together, in different plots
figure;
subplot(2, 2, 1), imshow(I);
title('Step-1: Load input image');
subplot(2, 2, 2), imshow(Igray);
title('Step-2: Conversion of input image to greyscale');
subplot(2, 2, 4), imshow(keeperBlobsImage);
title("Final Image");
subplot(2, 2, 3), imshow(Igray);
title('Step-2: Outlined');
axis image;
hold on
boundaries = bwboundaries(keeperBlobsImage);
numberOfBoundaries = size(boundaries, 1);
for k = 1 : numberOfBoundaries
    thisBoundary = boundaries{k};
    plot(thisBoundary(:,2), thisBoundary(:,1), 'LineWidth', 2);
end
hold off