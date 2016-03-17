/*************************************************************************
    > File Name: txt2bin.cpp
    > Author: Felix
    > Mail: admin@crazyqq.com 
    > Created Time: 2016å¹?3æœ?0æ—?13:38:40
 ************************************************************************/

#include <iostream>
#include <fstream>
#include "cxxopts.hpp"

enum CONVERTER_TYPE
{
	CONVERTER_TYPE_AUTO,
	CONVERTER_TYPE_BIN2TEXT,
	CONVERTER_TYPE_TEXT2BIN
};

class TxtBinConverter
{
private:
	std::ifstream	inFileStream;
	std::ofstream	outFileStream;
	CONVERTER_TYPE	cType;

	void autoCheckType() {
		unsigned char readData;	
		if ( (CONVERTER_TYPE_AUTO != cType)
 			|| (!inFileStream.good()) ) {
			return;
		}

		while (inFileStream.eof()) {
			readData = inFileStream.get();
			if (!isText(readData)) {
				cType = CONVERTER_TYPE_BIN2TEXT;
				printf("readData=0x%x\r\n",readData);
				break;
			}
		}

		inFileStream.clear(); //clear error flags
		inFileStream.seekg(0, std::ios::beg); //set the file get pointer back to the beginning
	}

	bool isText(unsigned char charValue) {
		if ( ( (charValue >= '0') && (charValue <= '9') )
			|| ( (charValue >= 'a') && (charValue <= 'z') ) 
			|| ( (charValue >= 'A') && (charValue <= 'Z') ) 
			|| ( '\n' == charValue)
			|| ( '\r' == charValue)
			|| ( ' ' == charValue)) {
			return true;
		} else {
			return false;
		}
	}

	unsigned char char2Txt(unsigned char bin_char) {
		unsigned txtValue = bin_char;
		/* 0~9 a~z */
		if ( (bin_char >= 0) && (bin_char <= 9) ) {
			txtValue += '0';
		} else if ( (bin_char >= 0x0A) && (bin_char <= 0x0F) ) {
			txtValue += ('A' - 0x0A);
		} else {
			txtValue = '0';
		}
		return txtValue;
	}

	void Txt2Bin() {
		unsigned char readData,left_half_byte;
		bool	read_left_flag = true;
		char	out_data  = 0;

		while ( inFileStream.good() )  
		{  
			readData = inFileStream.get();

			/* 0~9 a~z */
			if ( (readData >= '0') && (readData <= '9') ) {
				readData -= '0';
			} else if ( (readData >= 'a') && (readData <= 'z') ) {
				readData -= ('a' - 0x0A);
			} else if ( (readData >= 'A') && (readData <= 'Z') ) {
				readData -= ('A' - 0x0A);
			} else {
				continue;
			}

			if (!read_left_flag) {
				out_data = (left_half_byte<<4 | readData);

				/* write to file */
				outFileStream.write(&out_data,1);
				read_left_flag = true;
			} else {
				left_half_byte = readData;
				read_left_flag = false;
			}
		}  
	}

	void Bin2Txt() {
		unsigned char readData;
		bool	read_left_flag = true;
		char	out_data,left_byte;

		while ( inFileStream.good() )  
		{  
			readData = inFileStream.get();

			left_byte = (readData >> 4) & 0x0F;
			readData = readData & 0x0F;

			/* write left byte */
			out_data = char2Txt(left_byte);
			/* write to file */
			outFileStream.write(&out_data,1);

			/* write right byte */
			out_data = char2Txt(readData);
			/* write to file */
			outFileStream.write(&out_data,1);
		}  
	}

public:	
	TxtBinConverter
		(const std::string& inFileName,const std::string& outFileName,CONVERTER_TYPE type=CONVERTER_TYPE_AUTO)	 
	{
		cType = type;
		outFileStream.open(outFileName, std::ios::out|std::ios::trunc|std::ios::binary);
		inFileStream.open(inFileName,std::ios::in);
		autoCheckType();
	}

	~TxtBinConverter( ) {
		inFileStream.close();
		outFileStream.close();
	}

	void start() {
		if (inFileStream.good() && outFileStream.good()) {
			std::cout<<"converter:";
			if (CONVERTER_TYPE_BIN2TEXT == cType) {
				std::cout<<"bin2text"<<std::endl;
				Bin2Txt();
			} else {
				std::cout<<"text2bin"<<std::endl;
				Txt2Bin();
			}
		} else {
			std::cout<<"error:input file or output file open error!"<<std::endl;
		}
	}	
};

using namespace std;

int main(int argc,char* argv[])
{
	ofstream out_file_stream;
	ifstream in_file_stream;
	CONVERTER_TYPE type = CONVERTER_TYPE_AUTO;

	cxxopts::Options options(argv[0], " - Conver bin2hex or hex2bin");

	try {
		options.add_options()
		("i,inputFile", "File name", cxxopts::value<std::string>())
		("o,outputFile", "File name", cxxopts::value<std::string>()->default_value("output"))
		("t,type", "Input file type:0 auto check,1 bin file,2 txt file", cxxopts::value<int>(), "TYPE")
		("d,debug", "Enable debug log")
		("h,help", "Show the help")
		;

		options.parse(argc, argv);
	} catch (const cxxopts::OptionException& e)
	{
		std::cout << "error parsing options: " << e.what() << std::endl;
		exit(1);
	}

	if (options.count("help")) {
		std::cout << options.help({"", "Group"}) << std::endl;
		exit(0);
	}

	if (options.count("inputFile")) {
		
	} else {
		cout<<"Where is input file?"<<endl;
		cout << options.help({"", "Group"}) << std::endl;
		exit(1);
	}

	if (!options.count("outputFile")) {
		//no output file?
	}
	
	if (options.count("type")) {
		type = (CONVERTER_TYPE)options["type"].as<int>();
	}

	cout<<"inputfile:"<<options["inputFile"].as<std::string>()<<endl;
	cout<<"outfile:"<<options["outputFile"].as<std::string>()<<endl;

	TxtBinConverter txtBinConver(options["inputFile"].as<std::string>(),options["outputFile"].as<std::string>(),type);

	txtBinConver.start();

    return 0;
}
