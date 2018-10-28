clear
clc

I = imread('images/lena.BMP');
figure(1);
imshow(I); %��ʾͼƬ
[line,row] = size(I);

%�������ˮӡ��Ϣ
watermark_len = 125*125;
watermark = rand(1, watermark_len);
for i = 1:watermark_len
    if watermark(i) < 0.5
        watermark(i) = -1;
    else
        watermark(i) = 1;
    end
end

%(������)������Ӧ����Ƶ���У�
% 1 -> 1 -1 -1 1    -1 -> -1 1 1 -1
spread_len = 4;
spread_1 = [1 -1 -1 1];
watermark_spread = zeros(1, spread_len*watermark_len);
for i = 1:watermark_len
    if watermark(i) == 1
        watermark_spread(spread_len*(i-1)+1 : spread_len*i) = spread_1;
    else
        watermark_spread(spread_len*(i-1)+1 : spread_len*i) = -1*spread_1; %ȡ�෴��
    end
end

%Ƕ��ˮӡ
%ȷ��ˮӡǶ���㷨�Ĳ���
alpha = 3;
b = 1;
lambda = 1;
%��ͼƬת�ã�ʵ�־����е�һά����(Ĭ������°���)
I_watermark = double(I');
for i = 1:watermark_len
    if spread_len*i < line*row %ͼƬ�Ƿ����������һbit��ˮӡ��Ϣ
        if watermark(i) == 1
            s = (I_watermark(spread_len*(i-1)+1:spread_len*i)*spread_1') / spread_len;
            s = round(s); %������������
            I_watermark(spread_len*(i-1)+1:spread_len*i) = I_watermark(spread_len*(i-1)+1:spread_len*i) + (alpha*b-lambda*s)*spread_1;
        else
            s = (I_watermark(spread_len*(i-1)+1:spread_len*i)*(-1*spread_1)') / spread_len;
            s = round(s);
            I_watermark(spread_len*(i-1)+1:spread_len*i) = I_watermark(spread_len*(i-1)+1:spread_len*i) + (alpha*b-lambda*s)*(-1*spread_1);
        end
    else
        disp('ͼƬ�޷�Ƕ�����е�ˮӡ��Ϣ');
    end
end

%��ʾ�Ѿ�Ƕ����ˮӡ��Ϣ��ͼƬ
figure(2);
imshow(uint8(I_watermark'));

%ˮӡ����ȡ
watermark_extracted = zeros(1, watermark_len);
for i = 1:watermark_len
    z = (I_watermark(spread_len*(i-1)+1:spread_len*i)*spread_1') / spread_len;
    if z > 0
        watermark_extracted(i) = 1;
    else
        watermark_extracted(i) = -1;
    end
end

%�ж�ˮӡ��ȡ��׼ȷ��
equal_count = 0;
for i = 1:watermark_len
    if watermark(i) == watermark_extracted(i)
        equal_count = equal_count + 1;
    end
end
pct = (equal_count/watermark_len)*100; %�����Ӧ�ٷֱ�
fprintf('ˮӡ��ȡ��ȷ��: %.2f%%\n', pct);