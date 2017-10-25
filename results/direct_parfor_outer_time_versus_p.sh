#!/bin/bash
# The above shows that it is a bash file

PS=$(seq 4)

for P in $PS; do
	bin/time_fourier_transform hpce.aes414.direct_fourier_transform_parfor_outer $P 30 "${P}, " > dump_P${P}.csv
done 

