clc
clear 
close all

fileID = fopen('perfModel.txt','r');

v = [16 32 64 128 256 512];

n = length(v)^3;


for ii  = 1:n
    
    % Skip Four lines
    line = fgetl(fileID);    line = fgetl(fileID);    line = fgetl(fileID);    line = fgetl(fileID);
    
    % Read in the line of interest. 
        line = fgetl(fileID);
        
        best(ii) = str2num(line(18:27));
        worst(ii) = str2num(line(36:45));
        mean(ii) = str2num(line(52:61));
        
    
        
end



ctr = 1;
for i = v
    for j = v 
        for k  =v
            dim(ctr,:) = [i j k];
            dimlabel{ctr} = [num2str(i),',',num2str(j)];
            ctr = ctr +1;
            
            
        end
    end
end


plot(1:36,best(dim(:,3) == 16))
set(gca,'XTickLabel',dimlabel(dim(:,3)==16),'XTick',1:36)
ylabel('Cycles per DOF')
title('Cycles per DOF for a IxJx16 matrix')


%%
fileID = fopen('perfModel2.txt','r');

n = length(v)^3;


for ii  = 1:400
    
    % Skip three lines
    line = fgetl(fileID);    line = fgetl(fileID);    line = fgetl(fileID);
    
    % Read in the line of interest. 
        line = fgetl(fileID);
        
        best(ii) = str2num(line(18:27));
        worst(ii) = str2num(line(36:45));
        mean(ii) = str2num(line(52:61));
        
    
        
end

ctr = 1;
for i = v
    for j = v 
        for k  =v
            dim(ctr,:) = [i j k];
            ctr = ctr +1;
        end
    end
end


best16 = best(1:100);
worst16 = worst(1:100);
mean16 = mean(1:100);


best32 = best(101:200);
worst32 = worst(101:200);
mean32 = mean(101:200);


best64 = best(201:300);
worst64 = worst(201:300);
mean64 = mean(201:300);


best128 = best(301:400);
worst128 = worst(301:400);
mean128 = mean(301:400);

max_it = 50;
figure
plot(1:max_it,best16(1:max_it),1:max_it,worst16(1:max_it), 1:max_it,mean16(1:max_it));
xlabel('Number of iterations')
ylabel('Cycles per DOF')
legend('Best','Worst','Average')
title('Performance vs. Iterations for a 16^3 grid')
set(gca,'FontSize',16)

figure
plot(1:max_it,best32(1:max_it),1:max_it,worst32(1:max_it),1:max_it,mean32(1:max_it));
xlabel('Number of iterations')
ylabel('Cycles per DOF')
legend('Best','Worst','Average')
title('Performance vs. Iterations for a 32^3 grid')
set(gca,'FontSize',16)

figure
plot(1:max_it,best64(1:max_it),1:max_it,worst64(1:max_it),1:max_it,mean64(1:max_it));
xlabel('Number of iterations')
ylabel('Cycles per DOF')
legend('Best','Worst','Average')
title('Performance vs. Iterations for a 64^3 grid')
set(gca,'FontSize',16)

figure
plot(1:max_it,best128(1:max_it),1:max_it,worst128(1:max_it),1:max_it,mean128(1:max_it));
xlabel('Number of iterations')
ylabel('Cycles per DOF')
legend('Best','Worst','Average')
title('Performance vs. Iterations for a 128^3 grid')
set(gca,'FontSize',16)




