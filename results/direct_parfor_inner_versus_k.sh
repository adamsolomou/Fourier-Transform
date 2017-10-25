#!/bin/bash
# The above shows that it is a bash file

KS=$(seq 16)

for K in $KS; do
	export HPCE_DIRECT_INNER_K=${K} 
	bin/time_fourier_transform hpce.aes414.direct_fourier_transform_parfor_inner 0 30 "${HPCE_DIRECT_INNER_K}, " > dump_${HPCE_DIRECT_INNER_K}.csv
done 