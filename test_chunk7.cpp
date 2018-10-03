#include "xdfwriter.h"

int main(int argc, char** argv) {
	XDFWriter w1("test_big.xdf"), w2("test_big_v11.xdf");
	const uint32_t sid = 0x02C0FFEE;
	const int blocks = argc > 1 ? std::stoi(argv[1]) : 30;
	const std::string footer(
	    "<?xml version=\"1.0\"?>"
	    "<info>"
	    "<first_timestamp>5.1</first_timestamp>"
	    "<last_timestamp>5.9</last_timestamp>"
	    "<sample_count>9</sample_count>"
	    "<clock_offsets>"
	    "<offset><time>50979.7660030605</time><value>-3.436503902776167e-06</value></offset>"
	    "</clock_offsets></info>");
	const std::string header("<?xml version=\"1.0\"?>"
	                         "<info>"
	                         "<name>SendDataC</name>"
	                         "<type>EEG</type>"
	                         "<channel_count>64</channel_count>"
	                         "<nominal_srate>1000</nominal_srate>"
	                         "<channel_format>double64</channel_format>"
	                         "<created_at>50942.723319709003</created_at>"
	                         "</info>");
	w1.write_stream_header(sid, header);
	w2.write_stream_header(sid, header);
	w1.write_boundary_chunk();
	w2.write_boundary_chunk();

	// write a single int16_t sample
	const uint32_t n_samples = 10 * 1000, n_channels = 64;
	std::vector<double> vals(n_samples * n_channels), timestamps(n_samples);
	for (auto i = 0u; i < n_samples; i++) {
		timestamps[i] = 5.1 + i * .001;
		for (auto chan = 0u; chan < n_channels; chan++)
			vals[n_channels * i + chan] = chan + i * .002;
	}

	for (int i = 0; i < blocks; ++i) {
		w1.write_boundary_chunk();
		w2.write_boundary_chunk();
		w1.write_data_chunk(sid, timestamps, vals.data(), n_samples, n_channels);
		w2.write_better_data_chunk(sid, timestamps, vals.data(), n_samples, n_channels);
	}
	w1.write_boundary_chunk();
	w2.write_boundary_chunk();
	w1.write_stream_offset(sid, 6, -.1);
	w2.write_stream_offset(sid, 6, -.1);

	w1.write_stream_footer(sid, footer);
	w2.write_stream_footer(sid, footer);
}
