fliplr(dec2bin(data.real_string_mask(1),64))

str = repmat({'This is a test" it is'},1,1e7);
str2 = uint8(jsonencode(str));

%length(str2) - floor(length(str2)/32)*32
str3 = char(str2);

N = 100;
ta = time_averager(N);
for i = 1:N
    data = tj(str2);
    ta.add(data);
end

ta.dm

length(str2) - length(m)*64

%Doesn't work due to dec2bin not working with large numbers
m = data.real_string_mask;
I = 1;
for i = 1:length(m)-1
    temp = m(i);
    temp2 = dec2bin_good(temp);
    I2 = strfind(temp2,'1');
    I3 = strfind(str2(I:I+63),'"');
    if ~isequal(I2,I3)
        error('not equal')
    end
    I = I + 64;
end


m = data.real_string_mask;
I = 1;
for i = 1:length(m)-1
    temp = m(i);
    temp2 = fliplr(dec2bin_good(temp,64));
    I2 = strfind(temp2,'1');
    I3 = strfind(str2(I:I+31),'"');
    if ~isequal(I2,I3)
        error('not equal')
    end
    I = I + 32;
end
