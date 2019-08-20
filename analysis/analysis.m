% analysis.m
%
% Basic script for plotting the data from the malaria simulation.

% TODO Convert all of these to proper parameters
file = "../out/monthly_data_0.txt";
startDate = "1990/1/1";
burnInDate = "2019/12/1";

% Find the number of days to discard for the model burn-in
days = daysact(datetime(startDate, 'InputFormat', 'yyyy/MM/dd'), ...
               datetime(burnInDate, 'InputFormat', 'yyyy/MM/dd'));
           
% Discard all of the burn-in data


data = load(file);

