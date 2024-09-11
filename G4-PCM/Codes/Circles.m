A = imread('Xrays\1.png');
% 20 y 500 sirve, con bright
[centers,radii, metric] = imfindcircles(A,[7 2000],ObjectPolarity="bright",EdgeThreshold=0.5);
B=im2gray(A);
imshow(B)
viscircles(centers,radii,'EdgeColor','b')
%% 
A = imread('Xrays\2.png');
% 20 y 500 sirve, con bright
[centers,radii, metric] = imfindcircles(A,[50 100],ObjectPolarity="bright",Sensitivity=0.98,EdgeThreshold=0.05)
B=im2gray(A);
imshow(B)
viscircles(centers,radii,'EdgeColor','b')
%% 
A = imread('Xrays\4.png');
% 20 y 500 sirve, con bright
[centers,radii, metric] = imfindcircles(A,[50 100],ObjectPolarity="bright",Sensitivity=0.98,EdgeThreshold=0.05)
B=im2gray(A);
imshow(B)
viscircles(centers,radii,'EdgeColor','b')
%% 
A = imread('Xrays\6.png');
% 20 y 500 sirve, con bright
[centers,radii, metric] = imfindcircles(A,[30 100],ObjectPolarity="bright",Sensitivity=0.9,EdgeThreshold=0.5,Method="TwoStage")
B=im2gray(A);
imshow(B)
viscircles(centers,radii,'EdgeColor','b')
%% 
A = imread('Xrays\7.png');
% 20 y 500 sirve, con bright
[centers,radii, metric] = imfindcircles(A,[50 100],ObjectPolarity="bright",Sensitivity=0.98,EdgeThreshold=0.2)
B=im2gray(A);
imshow(B)
viscircles(centers,radii,'EdgeColor','b')
%% 
A = imread('Xrays\8.png');
% 20 y 500 sirve, con bright
[centers,radii, metric] = imfindcircles(A,[50 100],ObjectPolarity="bright",Sensitivity=0.98,EdgeThreshold=0.05)
B=im2gray(A);
imshow(B)
viscircles(centers,radii,'EdgeColor','b')
%% 
A = imread('Xrays\9.png');
% 20 y 500 sirve, con bright
[centers,radii, metric] = imfindcircles(A,[30 100],ObjectPolarity="dark",Sensitivity=0.94,EdgeThreshold=0.6,Method="TwoStage")
B=im2gray(A);
imshow(B)
viscircles(centers,radii,'EdgeColor','b')