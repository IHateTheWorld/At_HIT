clear
clc

I = imread('images/lena.BMP');
figure(1);
imshow(I); %��ʾͼƬ
[line,row] = size(I);

%������
lighttable=...
    [16 11 10 16 24 40 51 61 ; 
    12 12 14 19 26 58 60 55 ; 
    14 13 16 24 40 57 69 56 ; 
    14 17 22 29 51 87 80 62 ; 
    18 22 37 56 68 109 103 77;
    24 35 55 64 81 104 113 92; 
    49 64 78 87 103 121 120 101; 
    72 92 95 98 112 100 103 99];

fun = @dct2; %��ɢ���ұ任DCT��matlab�ڲ�����
DCT=blkproc(I,[8 8],fun); %�ֿ����DCT����

%����
quantize_DCT = DCT;
for i=1:line/8
    for j=1:row/8
        quantize_DCT(8*(i-1)+1:8*i, 8*(j-1)+1:8*j) = DCT(8*(i-1)+1:8*i, 8*(j-1)+1:8*j) ./ lighttable;
    end
end
quantize_DCT = round(quantize_DCT); %���������ȡ������������

jteg_DCT = quantize_DCT; %���������Ľ�����������DCTϵ������
f3_DCT = quantize_DCT;

%����������Ϣ
bit_len = 500;
bits_info = round(rand(1,bit_len));

%ʹ��Jsteg�㷨������д
line_pos = 1;
row_pos = 1;
i = 1;
while i<=bit_len
    if jteg_DCT(line_pos,row_pos)~=0 && jteg_DCT(line_pos,row_pos)~=1 && jteg_DCT(line_pos,row_pos)~=-1
        if mod(jteg_DCT(line_pos,row_pos),2)==0 && bits_info(i)==1
            if jteg_DCT(line_pos,row_pos) < 0
                jteg_DCT(line_pos,row_pos) = jteg_DCT(line_pos,row_pos) - 1; %���Ա���-2���-1�����
            else
                jteg_DCT(line_pos,row_pos) = jteg_DCT(line_pos,row_pos) + 1; %���Ա���2���1�����
            end
        elseif mod(jteg_DCT(line_pos,row_pos),2)==1 && bits_info(i)==0
            if jteg_DCT(line_pos,row_pos) < 0
                jteg_DCT(line_pos,row_pos) = jteg_DCT(line_pos,row_pos) + 1;
            else
                jteg_DCT(line_pos,row_pos) = jteg_DCT(line_pos,row_pos) - 1;
            end
        end
        i = i + 1;
    end
    row_pos = row_pos + 1;
    if row_pos == row + 1
        row_pos = 1;
        if line_pos < line
            line_pos = line_pos + 1;
        else
            disp('Jsteg��д: ��Ҫ������������࣬�����޷�����');
            break;
        end
    end
end

%��ȡJsteg�㷨�е���д��Ϣ
Jsteg_bits = zeros(1,bit_len);
read_len = 1;
line_pos = 1;
row_pos = 1;
while read_len <= bit_len
    if jteg_DCT(line_pos,row_pos)~=0 && jteg_DCT(line_pos,row_pos)~=1 && jteg_DCT(line_pos,row_pos)~=-1
        if mod(jteg_DCT(line_pos,row_pos),2)==0
            Jsteg_bits(read_len) = 0;
        else
            Jsteg_bits(read_len) = 1;
        end
        read_len = read_len + 1;
    end
    row_pos = row_pos + 1;
    if row_pos == row + 1
        row_pos = 1;
        if line_pos < line
            line_pos = line_pos + 1;
        else
            disp('Jsteg��д: д���������Ϣ����ȱʧ');
            break;
        end
    end
end

if isequal(bits_info,Jsteg_bits)
    disp('Jsteg��д: ��ȡ��д��һ��');
else
    disp('Jsteg��д: ��ȡ��д�벻һ��');
end

%Jsteg��ʾ����������Ϣ��ͼ��
Jsteg_m = jteg_DCT;
for i=1:line/8
    for j=1:row/8
        Jsteg_m(8*(i-1)+1:8*i, 8*(j-1)+1:8*j) = Jsteg_m(8*(i-1)+1:8*i, 8*(j-1)+1:8*j) .* lighttable;
    end
end
fun2 = @idct2; %��DCT�任��matlab�ڲ�����
Jsteg_m=blkproc(Jsteg_m,[8 8],fun2); %�ֿ���з�DCT�任����
Jsteg_m = uint8(round(Jsteg_m));
figure(2);
imshow(Jsteg_m);

%ʹ��f3�㷨������д
line_pos = 1;
row_pos = 1;
i = 1;
while i<=bit_len
    if f3_DCT(line_pos,row_pos)~=0
        if (f3_DCT(line_pos,row_pos)==1 || f3_DCT(line_pos,row_pos)==-1) && bits_info(i)==0
            f3_DCT(line_pos,row_pos) = 0; %��Ϊ��Ч
        else
            if (mod(f3_DCT(line_pos,row_pos),2)==0 && bits_info(i)==1) || (mod(f3_DCT(line_pos,row_pos),2)==1 && bits_info(i)==0)
                %����ֵ��һ�����Ų���
                if f3_DCT(line_pos,row_pos) < 0
                    f3_DCT(line_pos,row_pos) = f3_DCT(line_pos,row_pos) + 1;
                elseif f3_DCT(line_pos,row_pos) > 0
                    f3_DCT(line_pos,row_pos) = f3_DCT(line_pos,row_pos) - 1;
                end
            end
            i = i + 1;
        end
    end
    row_pos = row_pos + 1;
    if row_pos == row + 1
        row_pos = 1;
        if line_pos < line
            line_pos = line_pos + 1;
        else
            disp('F3��д: ��Ҫ������������࣬�����޷�����');
            break;
        end
    end
end

%��ȡf3�㷨�е���д��Ϣ
f3_bits = zeros(1,bit_len);
read_len = 1;
line_pos = 1;
row_pos = 1;
while read_len <= bit_len
    if f3_DCT(line_pos,row_pos)~=0
        if mod(f3_DCT(line_pos,row_pos),2)==0
            f3_bits(read_len) = 0;
        else
            f3_bits(read_len) = 1;
        end
        read_len = read_len + 1;
    end
    row_pos = row_pos + 1;
    if row_pos == row + 1
        row_pos = 1;
        if line_pos < line
            line_pos = line_pos + 1;
        else
            disp('F3��д: д���������Ϣ����ȱʧ');
            break;
        end
    end
end

if isequal(bits_info,f3_bits)
    disp('F3��д: ��ȡ��д��һ��');
else
    disp('F3��д: ��ȡ��д�벻һ��');
end

%F3��ʾ����������Ϣ��ͼ��
f3_m = f3_DCT;
for i=1:line/8
    for j=1:row/8
        f3_m(8*(i-1)+1:8*i, 8*(j-1)+1:8*j) = f3_m(8*(i-1)+1:8*i, 8*(j-1)+1:8*j) .* lighttable;
    end
end
fun2 = @idct2; %��DCT�任��matlab�ڲ�����
f3_m=blkproc(f3_m,[8 8],fun2); %�ֿ���з�DCT�任����
f3_m = uint8(round(f3_m));
figure(3);
imshow(f3_m);
