% MATLAB script for Assessment Item-1
% Task-1
clear; close all; clc;

% Step-1: Load input image
I = imread('Zebra.jpg');

% Step-2: Conversion of input image to grey-scale image
Igray = rgb2gray(I);

% gets the size of the matirx in the form of m n and oldSize
[m,n] = size(Igray);
oldSize = size(Igray);

%scale values are produced, desired size divided by the current
s1 = 1668/m;
s2 = 1836/n;
scale = [s1 s2];

%size of the new image is calculated by scaling up the old size
%max get the maximum number of elements in the array
%floor then rounds towards the negative
%s1*m s2*n
newSize = max(floor(scale.*oldSize(1:2)), 1);

%min gets the minium number of elements in an array
%round rounds to the nearest integer 
%'./' is a scalar divide, does the right side first
%':' goes between the values, so 1 to newSize(1), which is 1668
%then it adds the 0.5 back at the end
%row index with go from 1 to the old image size, but stretched
%over the new image size (1-556 via 1-1668)
rowIndex = min(round(((1:newSize(1))-0.5)./scale(1)+0.5),m);
colIndex = min(round(((1:newSize(2))-0.5)./scale(2)+0.5),n);

%creates the new image, using the indices created, therefore
%stretching the image
outputImage = Igray(rowIndex,colIndex, :);




%Show the images
%puts them all onto the same figure, plotted together
%axis is on to see the number of pixels
%all are titles so you can see what each step does
figure;
subplot(2, 2, 1),imshow(I);
axis on
title('Step-1: Load input image');

subplot(2, 2, 2),imshow(Igray);
axis on
title('Step-2: Conversion of input image to greyscale');

subplot(2, 2, 3),imshow(outputImage);
axis on
title('Step-3: Resize the image with Nearest Neighbour');

%Bilinear
for row = 1:m
    for col = 1:n
        newImage(row*s1, col*s2) = Igray(row,col); 
    end
end

figure;
imshow(newImage);

[a,b] = size(newImage);
for row = 1:m-1
    for col = 1:n-1
        for nrow = 1:a -1
            for ncol = 1:b-1
                rowPosition = row * scale(1);
                colPosition = col * scale(1);
                x = rowPosition + row;
                x1 = row;
                x2 = rowPosition + scale(1);

                something1 = newImage(rowPosition, colPosition + scale(1));
                something2 = newImage(rowPosition, colPosition);
                something3 = newImage(rowPosition  + scale(1), colPosition);
                something4 = newImage(rowPosition + scale(1), colPosition + scale(1));
                temp1 = ((x2 - x)/(x2-x))*something1 + ((x-x1)/(x2-x1))*something2;
                temp2 = ((x2 - x)/(x2-x))*something3 + ((x-x1)/(x2-x1))*something4;
                
                y = colPosition + nrow;
                y1 = colPosition;
                y2 = colPosition + scale(1);
                
                temp3(rowPosition+ nrow, colPosition+ncol) = (((y2-y)/(y2-y1))*temp1) + ((((y-y1)/(y2-y2))*temp2));
            end
         end
    end
end

figure;
imshow(temp3);
title("dfdsfdf");