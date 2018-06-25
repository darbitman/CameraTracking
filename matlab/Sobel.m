close all;
clear;
src = imread('guitar.png');
figure, imshow(src);
title('Original image');
srcGrayscale(:, :, :) = (src(:, :, 1) + src(:, :, 2) + src(:, :, 3)) / 3;
srcLuminosity(:, :, :) = 0.21*src(:, :, 1) + 0.72*src(:, :, 2) + 0.07*src(:, :, 3);
figure, imshow(srcLuminosity);
title('Grayscale image using luminosity');

%%% Gauss filter to smooth edges
srcLuminosity = imgaussfilt(srcLuminosity);
figure, imshow(srcLuminosity);
title('Gauss blur on grayscale');

%%% kernels to find partial derivatives
xKernel = [-1 0 1;
           -2 0 2;
           -1 0 1;];
yKernel = xKernel';

%%% Compute partial derivatives
xEdges = conv2(srcLuminosity, xKernel);
xEdges = xEdges(2:size(xEdges,1)-1, 2:size(xEdges,2)-1);
yEdges = conv2(srcLuminosity, yKernel);
yEdges = yEdges(2:size(yEdges,1)-1, 2:size(yEdges,2)-1);
figure, imshow(xEdges);
title('X edges');
figure, imshow(yEdges);
title('Y edges');

mag = sqrt(xEdges.^2 + yEdges.^2);
figure, imshow(mag);
title('Edge mag');
figure, imshow(xEdges + yEdges);