n=30;
t_beta= zeros(n,45);
fileID = fopen('beta.txt','w');
for i = 1:n
    content = fileread(sprintf('input_BF_%d.yml',i));
    beta = regexp(content,'(beta_by_location: \[)(.*)\]','tokens','dotexceptnewline');
    fprintf(fileID,'%s\n',beta{1}{1,2});
end

fclose(fileID);