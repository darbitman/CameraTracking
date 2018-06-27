%%% function computes gradient histograms given cell parameters
%%% matrix M, starting position at (r,c) and dimensions of cell is (h,w)
%%% put results in one of b bins
function H = GetHistogram(mag, ang, r, c, h, w, b)
deltaAng = 181 / b;
H = zeros(1,1,b);
    for j = c:w
        for k = r:h
            %%% compute weighted gradient
            w = ang(k,j) * mag(k,j);
            
            %%% compute bin
            bin = abs(fix(ang(k,j) / deltaAng)) + 1;
            H(1,1,bin) = H(1,1,bin) + w;
        end
    end
end