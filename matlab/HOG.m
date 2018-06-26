close all;
clear;
%%% Get input image %%%
src = imread('guitar.png');
figure, imshow(src);
title('Original image');

%%% convert to grayscale
graySrc(:, :, :) = 0.21*src(:, :, 1) + 0.72*src(:, :, 2) + 0.07*src(:, :, 3);
figure, imshow(graySrc);
title('Grayscale image using luminosity');

%%% compute gradients
xKernel = [1 0 -1];
yKernel = xKernel';
dx = conv2(xKernel, double(graySrc));
dx = dx(:, 2:size(dx,2)-1);
dy = conv2(yKernel, double(graySrc));
dy = dy(2:size(dy,1)-1, :);

%%% convert vectors to polar
mag = sqrt(dx.^2 + dy.^2);
ang = atan2(dy,dx) * 360 / (2 * pi);

%%% compute number of cells
horizCells = size(src,2) / 8;
vertCells = size(src,1) / 8;


numBins = 9;
hist = zeros(vertCells, horizCells, numBins);