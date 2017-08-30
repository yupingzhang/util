// egviewer.cpp

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <iomanip>
#include <regex>

using namespace std;


void loader(const char* input, vector<vector<double> >& eigenvectors)
{
	ifstream in(input, std::ifstream::in);
	if(!in)
	{
		cerr << "Cannot open " << input << endl; exit(1);
    }
    else
    {
    	cout << "Loading " << input << endl;
    }

    string line;
    int count = 0;
    while (getline(in, line))
    {   
    	if (line.substr(0,1) != "#")
        {
        	vector<double> ev;
    	    // istringstream s(line);
            istringstream s{std::regex_replace(line, std::regex{R"(\[|\]|,)"}, " ")};
    	    copy(istream_iterator<double>(s), istream_iterator<double>(), back_inserter(ev));
		    eigenvectors.push_back(ev);
        }
    }
}

void generateGrid(const char* input, const char* output, vector<double> eigenvector, float scale, float mesh_scale)
{
	ifstream in(input, std::ifstream::in);
	if(!in)
	{
		cerr << "Cannot open " << input << endl; exit(1);
    }
    else
    {
    	cout << "Loading " << input << endl;
    }

    string line;
    double x, y, z;
    int a, b, c;
    vector<vector<double> > vert;
    vector<vector<int> > face;

    while (getline(in, line))
    {   
        if (line.substr(0,2) == "v ")
        {
            istringstream s(line.substr(2));
            s >> x; s >> y; s >> z;
            vector<double> v = {x, y, z};
            vert.push_back(v);
        }
        else if (line.substr(0,2) == "f ")
        {
            istringstream s(line.substr(2));
            s >> a; s >> b; s >> c;
            // a--; b--; c--;        // because the face index in obj start at 1
            vector<int> f = {a, b, c};
            face.push_back(f);
        }
        else
        {
            /* ignore #, f, o */
        }
    }
    ofstream out(output, ofstream::out);

    int total = vert.size();
    out.precision(16);
    out.setf( std::ios::fixed, std:: ios::floatfield );
    
    for (int i = 0; i < total; ++i)
    {
        out << "v " << vert[i][0] * mesh_scale << " " << vert[i][1] * mesh_scale << " " << eigenvector[i] * scale << endl;
    }

    for (int i = 0; i < face.size(); ++i)
    {
        out << "f " << face[i][0] << " " << face[i][1] << " " << face[i][2] << endl;
    }

    out.close();

}


int main(int argc, char const *argv[])
{
	if (argc < 4)
	{
		cout << "usage: ./e input_eigen input_mesh output_prefix scale mesh_scale" << endl;
	}
	const char* input = argv[1];
	const char* input_mesh = argv[2];
	const char* output = argv[3];
    float scale = 1.0;
    std::stringstream ss(argv[4]);
    if (!ss.fail()) {
        ss >> scale;
    }

    float mesh_scale = 1.0;
    std::stringstream ss2(argv[5]);
    if (!ss2.fail()) {
        ss2 >> mesh_scale;
    }
    
	vector<vector<double> > eigenvectors;
	loader(input, eigenvectors);

	int num = eigenvectors.size();
    //debug
    cout << "eigenvector size: " << eigenvectors[0].size() << endl;

	for (int i = 0; i < num; ++i)
	{
		ostringstream ss;
        ss << std::setw( 3 ) << std::setfill( '0' ) << i;
		string outputfile(output + ss.str() + string(".obj"));
		// 
		cout << "write to " << outputfile << endl;
		generateGrid(input_mesh, outputfile.c_str(), eigenvectors[i], scale, mesh_scale);
	}

	return 0;
}