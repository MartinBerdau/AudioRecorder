classdef PegelmesserPrototyp < handle
    % Idee: Objekt erstellen un mit Funktion immer wieder mit neuen Daten
    % feeden.
    
    
    properties % in c++: alle protected
        m_tau
        m_fs
        m_data
        
        m_alpha
    end
    
    methods
        
        function obj = PegelmesserPrototyp(tau,fs)
            obj.m_tau = tau;
            obj.m_fs = fs;
            obj.m_data = zeros(1,2); % in c++: Speicher der Länge 2 anlegen
            obj.setAlpha();
        end
        
        function setTau(obj,tau)
            obj.m_tau = tau;
            obj.setAlpha();
        end
        
        function setFs(obj,fs)
            obj.m_fs = fs;
            obj.setAlpha();
        end
        
        function setAlpha(obj)
            obj.m_alpha = exp(-2/(obj.m_tau * obj.m_fs));
        end
        
        function RMS = updateRMS(obj,curVal)
            obj.m_data(2) = obj.m_data(1); %m_data.at(2)
            obj.m_data(1) =...
                (1-obj.m_alpha)*curVal + obj.m_alpha*obj.m_data(2);
            % y(k) = (1-alpha)*x(k) + alpha*y(k-1)
            RMS = 20*log10(obj.m_data(1));
        end
        
    end
        
end