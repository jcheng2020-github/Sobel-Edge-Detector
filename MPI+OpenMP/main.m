img = imread("image2024.jpg");
gray = rgb2gray(img);
resized_gray = imresize(gray, [5000, 5000]);
writematrix(resized_gray, 'input.txt', 'Delimiter', 'tab');
figure, imshow(resized_gray)