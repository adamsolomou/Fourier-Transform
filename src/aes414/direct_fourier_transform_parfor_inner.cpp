	#include "fourier_transform.hpp"

	#include <cmath>
	#include <cassert>
	#include <cstdlib>

	#include "tbb/parallel_for.h"

	namespace hpce
	{
		namespace aes414
		{

	class direct_fourier_transform_parfor_inner
		: public fourier_transform
	{
	protected:
		/*! We can do any size transform */
		virtual size_t calc_padded_size(size_t n) const
		{
			return n;
		}

		virtual void forwards_impl(
			size_t n,	const complex_t &wn,
			const complex_t *pIn,
			complex_t *pOut
		) const
		{
			assert(n>0);

			const real_t PI2=2*3.1415926535897932384626433832795;

			// = -i*2*pi / n
			complex_t neg_im_2pi_n=-complex_t(0, 1)*PI2 / (real_t)n;

			// Fill the output array with zero
			std::fill(pOut, pOut+n, 0);

			// Read enviromental variable
			int K;
			char *v=getenv("HPCE_DIRECT_INNER_K");
			if (v==NULL){
				//printf("HPCE_DIRECT_INNER_K is not set .\n");
				K=8; //set default chunk size
			}else{
				//printf("HPCE_DIRECT_INNER_K = %s\n", v);
				K=atoi(v); //convert string to integer
			}

			for(size_t ii=0;ii<n;ii++){
				//tbb::parallel_for(size_t(0), n, [&](unsigned kk){
				tbb::parallel_for(tbb::blocked_range<unsigned>(0,n,K), [&](const tbb::blocked_range<unsigned> &chunk){
					for(unsigned kk=chunk.begin(); kk!=chunk.end(); kk++){	
						pOut[kk] += pIn[ii] * exp( neg_im_2pi_n * (double)kk * (double)ii );
					}
				}, tbb::simple_partitioner());
			}
		}

		virtual void backwards_impl(
			size_t n,	const complex_t &/*wn*/,
			const complex_t *pIn,
			complex_t *pOut
		) const
		{
			assert(n>0);

			const real_t PI2=2*3.1415926535897932384626433832795;

			// = i*2*pi / n
			complex_t im_2pi_n=complex_t(0, 1)*PI2 / (real_t)n;
			const real_t scale=real_t(1)/n;

			// Fill the output array with zero
			std::fill(pOut, pOut+n, 0);

			int K;
			char *v=getenv("HPCE_DIRECT_INNER_K");
			if (v==NULL){
				//printf("HPCE_DIRECT_INNER_K is not set .\n");
				K=8; //set default chunk size to 8
			}else{
				//printf("HPCE_DIRECT_INNER_K = %s\n", v);
				K=atoi(v); //convert string to integer
			}

			for(size_t ii=0;ii<n;ii++){
				//tbb::parallel_for(size_t(0), n, [&](unsigned kk){
				tbb::parallel_for(tbb::blocked_range<unsigned>(0,n,K), [&](const tbb::blocked_range<unsigned> &chunk){
					for(unsigned kk=chunk.begin(); kk!=chunk.end(); kk++){
						pOut[kk] += pIn[ii] * exp( im_2pi_n * (double)kk * (double)ii ) * scale;
					}
				}, tbb::simple_partitioner());
			};
		}

	public:
		virtual std::string name() const
		{ return "hpce.aes414.direct_fourier_transform_parfor_inner"; }

		virtual bool is_quadratic() const
		{ return true; }
	};

	std::shared_ptr<fourier_transform> Create_direct_fourier_transform_parfor_inner()
	{
		return std::make_shared<direct_fourier_transform_parfor_inner>();
	}
			
		};

	}; // namespace hpce
