clear;

initial_beta = [0.20204656,0.19399182,0.40204278,0.2452158,0.2228225,0.2587702,0.5206821,0.32452938,0.13474298,0.20767397,0.10905786,0.1391594,0.18279943,0.15081643,0.11222805,0.16579913,0.37831125,0.37507784,0.17315799,0.24359089,0.1571756,0.08519615,0.2073447,0.5408001,1.2264812,10.059139,0.7274604,0.1320237,0.32800967,0.2524852,0.23189375,0.1241698,0.12577781,0.11793135,0.12973739,0.12446517,0.12928228,1.1482792,0.22726184,0.46626127,0.57320553,0.3019596,0.17320079,0.64085674,0.3530287];
from_beta = initial_beta - initial_beta /2;
to_beta = initial_beta + initial_beta /2;

n=30;

t_beta= zeros(n,45);
for i = 1:n
    t_beta(i,:) = unifrnd(from_beta,to_beta);
end

filecontent = fileread('input_BF.yml');
for i = 1:n
    t = num2str(t_beta(i,:),'%f,');
    t = sprintf('%s%s','$1[',t(1:end-1),']');
    
    newfilecontent = regexprep(filecontent,'(beta_by_location: )(.*)',t,'dotexceptnewline');
    fid = fopen(sprintf('input_BF_%d.yml',i), 'w');
    fwrite(fid, newfilecontent);
    fclose(fid);
end

