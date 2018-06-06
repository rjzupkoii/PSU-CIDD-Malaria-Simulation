close all;
clear;

cr_colors={[0.0 0.6 0.0], [0.84 0.52 0.24], [0.8 0.12 0.12]};
cr_colors_fill={[0.35 0.9 0.35], [1.0 0.82 0.49], [1.0 0.43 0.43]};

genotype_id = 0;
DD = [1 2 3 4 5 6];

% % %generatedata
% for dosing_day_i = 1:1:size(DD,2);
%     dosing_day = DD(dosing_day_i);
%     output_file = sprintf('individuals_ASMQ_%d.txt',dosing_day);
%     [status, result] = system(sprintf('dist\\Release\\MinGW-Windows\\pkpdsimulation.exe -i basic_config.yml -o %s -genotype_id %d -dosing_day %d',output_file, genotype_id, dosing_day));
% end

%plot


log_parasite_density_detectable =   1.000;
log_parasite_density_cured =       -4.699+0.001;

% figure 1: parasite density
figure;
for dosing_day_i = 1:1:size(DD,2);
    dosing_day = DD(dosing_day_i);
    subaxis(size(DD,2)/2,2,dosing_day_i, 'Spacing', 0.03, 'Padding', 0.02, 'Margin', 0.05);
    output_file = sprintf('individuals_%d.txt',dosing_day);
    
    A = dlmread(output_file);
    B = transp(A);
    X = [1 2 3 4 8 15 22 29];
    line(  [-1 30],[1 1], 'Color', cr_colors{3}, 'LineWidth', 1);hold on;
    ws = 1.5;
    if(dosing_day_i >=5)
        boxplot( B(:,X) ,'position', X-1, 'label', X-1,'whisker', ws);
        xlabel('Time (days)');
    else
        boxplot( B(:,X),'position', X-1, 'label',{'','','','','','','',''},'whisker', ws);
    end
    
    if (dosing_day_i ==1 || dosing_day_i ==3 || dosing_day_i ==5)
        ylabel('Asexual density (log)');
    else
        set(gca,'YTickLabel',{});
    end
    
    set(gca,'XTick',X-1);
    axis([-0.5 29 -5 5.5]);
    set(gca,'XTickLabel',{});
    
    title({sprintf('%d-day-dosing',dosing_day)});
    
    box off;
end


% figure 2:
figure;
for dosing_day_i = 1:1:size(DD,2);
    dosing_day = DD(dosing_day_i);
    subaxis(size(DD,2)/2,2,dosing_day_i, 'Spacing', 0.05, 'Padding', 0.01, 'Margin', 0.05);
    output_file = sprintf('individuals_%d.txt',dosing_day);
    A = dlmread(output_file);
    B = transp(A);
    X = [1 2 3 4 8 15 22 29];
    
    C = [zeros(size(B,1),1) B];
    C = C(:,1:size(B,2));
    
    D = C- B;
    
    
    plot(X-1, [sum(B(:,X)>log_parasite_density_detectable)*100 / size(B,1); sum(B(:,X)>log_parasite_density_cured)*100/size(B,1)]);
    
    set(gca,'XTick',X-1);
    
    set(gca,'XTickLabel',{});
    
    
    if (dosing_day_i ==1 || dosing_day_i ==3 || dosing_day_i ==5)
        ylabel('Percent with Detectable Parasites');
    else
        set(gca,'YTickLabel',{});
    end
    
    
    if(dosing_day_i >=5)
        set(gca,'XTickLabel',X-1);
        xlabel('Time (days)');
    else
        set(gca,'XTickLabel',{});
    end
    
    if dosing_day_i ==6
        legend('Microscopic', 'PCR');
    end
    
    set(gca,'xscale','log');
    axis([1 29 0 101]);
    
    title({sprintf('%d-day-dosing',dosing_day)},'FontWeight','Normal');
    box off;
end

% figure 3: PRR
figure;
for dosing_day_i = 1:1:size(DD,2);
    dosing_day = DD(dosing_day_i);
    subaxis(size(DD,2)/2,2,dosing_day_i, 'Spacing', 0.03, 'Padding', 0.02, 'Margin', 0.05);
    output_file = sprintf('individuals_%d.txt',dosing_day);
    A = dlmread(output_file);
    B = transp(A);
    X = [2:15 22 29];
    
    C = [zeros(size(B,1),1) B];
    C = C(:,1:size(B,2));
    
    D = C- B;
    D(D <0.01) = NaN;
    %boxplot(D);
    
    if(dosing_day_i >=5)
        boxplot( D(:,X),'position', X-1, 'label', X-1);
        xlabel('Time (days)');
    else
        boxplot( D(:,X),'position', X-1, 'label',{'','','','','','','','','','','','','','','',''});
    end
    
    
    
    
    if (dosing_day_i ==1 || dosing_day_i ==3 || dosing_day_i ==5)
        ylabel('log(PRR)');
    else
        set(gca,'YTickLabel',{});
    end
    
    if(dosing_day_i >=5)
        
        xlabel('Time (days)');
    else
        set(gca,'XTickLabel',{});
    end
    
    
    title({sprintf('%d-day-dosing',dosing_day)});
    
end