close all;
clear;
%%% Get input image %%%
src = imread('guitar.png');
src = src(25:32,25:32,:);
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
pixelsW = 8;
pixelsH = 8;
horizCells = size(src,2) / pixelsW;
vertCells = size(src,1) / pixelsH;


numBins = 9;
hist = zeros(vertCells, horizCells, numBins);
for r = 1:vertCells
    for c = 1:horizCells
        i = (c - 1)*pixelsW + 1;
        j = (r - 1)*pixelsH + 1;
        hist(r,c,:) = GetHistogram(mag, ang, j, i, pixelsH, pixelsW, 9);
    end
end