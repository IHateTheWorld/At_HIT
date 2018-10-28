clear
clc

I = imread('images/lena.BMP');
figure(1);
imshow(I); %显示图片
[line,row] = size(I);

%随机生成水印信息
watermark_len = 125*125;
watermark = rand(1, watermark_len);
for i = 1:watermark_len
    if watermark(i) < 0.5
        watermark(i) = -1;
    else
        watermark(i) = 1;
    end
end

%(好像不用)生成相应的扩频序列：
% 1 -> 1 -1 -1 1    -1 -> -1 1 1 -1
spread_len = 4;
spread_1 = [1 -1 -1 1];
watermark_spread = zeros(1, spread_len*watermark_len);
for i = 1:watermark_len
    if watermark(i) == 1
        watermark_spread(spread_len*(i-1)+1 : spread_len*i) = spread_1;
    else
        watermark_spread(spread_len*(i-1)+1 : spread_len*i) = -1*spread_1; %取相反数
    end
end

%嵌入水印
%确定水印嵌入算法的参数
alpha = 3;
b = 1;
lambda = 1;
%将图片转置，实现矩阵按行的一维访问(默认情况下按列)
I_watermark = double(I');
for i = 1:watermark_len
    if spread_len*i < line*row %图片是否可以容纳下一bit的水印信息
        if watermark(i) == 1
            s = (I_watermark(spread_len*(i-1)+1:spread_len*i)*spread_1') / spread_len;
            s = round(s); %进行四舍五入
            I_watermark(spread_len*(i-1)+1:spread_len*i) = I_watermark(spread_len*(i-1)+1:spread_len*i) + (alpha*b-lambda*s)*spread_1;
        else
            s = (I_watermark(spread_len*(i-1)+1:spread_len*i)*(-1*spread_1)') / spread_len;
            s = round(s);
            I_watermark(spread_len*(i-1)+1:spread_len*i) = I_watermark(spread_len*(i-1)+1:spread_len*i) + (alpha*b-lambda*s)*(-1*spread_1);
        end
    else
        disp('图片无法嵌入所有的水印信息');
    end
end

%显示已经嵌入了水印信息的图片
figure(2);
imshow(uint8(I_watermark'));

%水印的提取
watermark_extracted = zeros(1, watermark_len);
for i = 1:watermark_len
    z = (I_watermark(spread_len*(i-1)+1:spread_len*i)*spread_1') / spread_len;
    if z > 0
        watermark_extracted(i) = 1;
    else
        watermark_extracted(i) = -1;
    end
end

%判断水印提取的准确率
equal_count = 0;
for i = 1:watermark_len
    if watermark(i) == watermark_extracted(i)
        equal_count = equal_count + 1;
    end
end
pct = (equal_count/watermark_len)*100; %计算对应百分比
fprintf('水印提取正确率: %.2f%%\n', pct);