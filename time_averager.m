classdef time_averager < handle
    %
    %   Class:
    %   json.utils.time_averager
    
    properties
        I = 0;
        fieldnames = {
            'step0'
            'step1'
        }
        step0
        step1
        
    end
    
    methods
        function obj = time_averager(N)
            for i = 1:length(obj.fieldnames)
                cur_name = obj.fieldnames{i};
                obj.(cur_name) = zeros(1,N);
            end
        end
        function add(obj,data)
            obj.I = obj.I + 1;
            I2 = obj.I;
            temp = data;
            for i = 1:length(obj.fieldnames)
                cur_name = obj.fieldnames{i};
                cur_name2 = ['time_' cur_name];
                try
                obj.(cur_name)(I2) = 1000*temp.(cur_name2);
                end
            end
        end
        function s = getMeans(obj)
            s = struct;
            for i = 1:length(obj.fieldnames)
                cur_name = obj.fieldnames{i};
                data = obj.(cur_name);
                s.(cur_name) = mean(data);
            end
        end
        function dm(obj)
            %dm - display means
            s = obj.getMeans();
            disp(s)
        end
        function s = getPercentages(obj)
            s = struct;
            for i = 1:length(obj.fieldnames)
                cur_name = obj.fieldnames{i};
                data = obj.(cur_name);
                s.(cur_name) = mean(data);
            end
            for i = 1:length(obj.fieldnames)
                cur_name = obj.fieldnames{i};
                s.(cur_name) = 100*s.(cur_name)./s.total_elapsed_time_mex;
            end
        end
        function dp(obj)
            %dp - display percents
            s = getPercentages(obj);
            disp(s)
        end
    end
end
 

