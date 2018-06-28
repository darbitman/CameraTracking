%%% function normalizes blocks
%%% pass in the histogram to normalize, given a certain blocksize (how many
%%% cells in the vertical/horizontal direction)
%%% number of blocks in the vertical and horizontal direction
function H = BlockNormalize(hist, blocksize, vBlocks, hBlocks)
H = zeros(size(hist));
for h = 1:hBlocks
    for v = 1:vBlocks
        subsetHist = hist(v:v+(blocksize-1), h:h+(blocksize-1),:);
        mag = sqrt(sum(sum(sum(subsetHist.^2))));
        H(v:v+(blocksize-1), h:h+(blocksize-1),:) = subsetHist / mag;
    end
end
end