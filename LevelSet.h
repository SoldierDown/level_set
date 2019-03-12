#ifndef LEVELSET_H
#define LEVELSET_H

#include <iostream>
#include <vector>
#include <eigen3/Eigen/Dense>
#include "Node.h"

#define X0 		100.0
#define Y0 		100.0
#define RADIUS 	40.0
#define XL		60
#define XR		140
#define YD		60
#define YU		140

using namespace std;
using namespace Eigen;

class LevelSet
{
private:
	/* data */
public:
	string mode;
	int rows, cols;
	double cell_width;
	double dt;
	double max;
	double max_err;
	vector<Node> nodes;
	LevelSet(string mode_);
	~LevelSet();
	//void Evolve();
	int Index(int i_, int j_);
	double NSquare(double x_);
	double PSquare(double x_);
	double Norm_2D(int i_, int j_);
	double Max(double x_, double y_);
	void Init();
	int S(double phi_);
	void Step();
	void SignedDistanceFunction(double err_);
	void Backup();
	double Distance(double x_, double y_);
};

LevelSet::LevelSet(string mode_): mode{mode_}, rows{200}, cols{200}, cell_width{1.0}, dt{0.1}, max{-100}, max_err{100}
{
}

LevelSet::~LevelSet()
{
}

void LevelSet::Init()
{
	double distance;
	double x, y;
	for(int i = 0; i < rows + 2; i++)
	{
		for(int j = 0; j < cols + 2; j++)
		{
			Node node;
			if(mode == "circle")
			{
				distance = Distance(i * cell_width, j * cell_width);
				if(distance < RADIUS)
				{
					node.phi_bu = -0.20;
				}
				else if(distance > RADIUS)
				{
					node.phi_bu = 0.20;
				}
				else
				{
					node.phi_bu = 0.0;
				}
			}
			else if(mode == "square")
			{
				x = i * cell_width;
				y = j * cell_width;
				if((x == XL && y >= YD && y <= YU)||(x == XR && y >= YD && y <= YU)||(y == YD && x >= XL && x <= XR)||(y == YU && x >= XL && x <= XR))
				{
					node.phi_bu = 0.0;
				}
				else if(x > XL && x < XR && y > YD && y < YU)
				{
					node.phi_bu = -0.20;
				}
				else
				{
					node.phi_bu = 0.20;
				}
			}
			nodes.push_back(node);
		}
	}

}

void LevelSet::SignedDistanceFunction(double err_)
{	
	cout << "max_err: " << max_err << endl;
	cout << "err_: " << err_ << endl;
	while(max_err > err_)
	{
		cout << max_err << endl;
		Step();
	}
}

int LevelSet::Index(int i_, int j_)
{
	if(i_ == rows) i_ = 0;
	if(i_ == -1) i_ = rows - 1;
	if(j_ == cols) j_ = 0;
	if(j_ == -1) j_ = cols - 1;
	return i_ * (cols + 2) + j_;
}

void LevelSet::Step()
{
	max = -100;
	max_err = -100;
	int id = 0;
	for(int i = 1; i <= rows; i++)
	{
		for(int j = 1; j <= cols; j++)
		{
			id = Index(i, j);
			nodes[id].phi = nodes[id].phi_bu - dt * S(nodes[id].phi_bu) * (Norm_2D(i,j) - 1);
			//cout << nodes[id].phi <<  "    "; 
			if(abs(nodes[id].phi) > max ) max = abs(nodes[id].phi);
			if(abs((nodes[id].phi - nodes[id].phi_bu)/nodes[id].phi) > max_err) max_err = abs(nodes[id].phi - nodes[id].phi_bu);
		}
	}
	for(int i = 1; i <= rows; i++)
	{
		nodes[Index(i,0)].phi = nodes[Index(i,1)].phi;
		nodes[Index(i,cols + 1)].phi = nodes[Index(i, cols)].phi;
	}

	for(int j = 1; j <= cols; j++)
	{
		nodes[Index(0, j)].phi = nodes[Index(1, j)].phi;
		nodes[Index(rows + 1, j)].phi = nodes[Index(rows, j)].phi;
	}
		                   
	nodes[Index(0, 0)].phi = 0.5 * (nodes[Index(0, 1)].phi + nodes[Index(1, 0)].phi);
	nodes[Index(0, cols + 1)].phi = 0.5 * (nodes[Index(1, cols + 1)].phi + nodes[Index(0, cols)].phi);
	nodes[Index(rows + 1, cols + 1)].phi = 0.5 * (nodes[Index(rows + 1, cols)].phi + nodes[Index(rows, cols + 1)].phi);
	nodes[Index(rows + 1, 0)].phi = 0.5 * (nodes[Index(rows, 0)].phi + nodes[Index(rows + 1, 1)].phi);

	Backup();
	cout << max << endl;
	cout << "Step() SUCCESS!" << endl;
}

double LevelSet::NSquare(double x_)
{
	if(x_ < 0) return x_ * x_;
	else return 0;
}

double LevelSet::PSquare(double x_)
{
	if(x_ > 0) return x_ * x_;
	else return 0;
}

int LevelSet::S(double phi_)
{
	if(phi_ > 0) return 1;
	else if(phi_ < 0) return -1;
	else return 0;
}

void LevelSet::Backup()
{
	int id = 0;
	for(int i = 0; i < rows + 2; i++)
	{
		for(int j = 0; j < cols + 2; j++)
		{
			id = Index(i,j);
			nodes[id].phi_bu = nodes[id].phi;// - dt * S(nodes[id].phi_bu) * (Vector2d(nabla_phi_x, nabla_phi_y).norm() - 1);
		}
	}
}

double LevelSet::Norm_2D(int i_, int j_)
{
	int id = Index(i_, j_);
	double a = (nodes[id].phi_bu - nodes[Index(i_-1, j_)].phi_bu) / cell_width;
	double b = (nodes[Index(i_ + 1, j_)].phi_bu - nodes[Index(i_, j_)].phi_bu) / cell_width;

	double c = (nodes[id].phi_bu - nodes[Index(i_, j_ - 1)].phi_bu) / cell_width;
	double d = (nodes[Index(i_, j_ + 1)].phi_bu - nodes[Index(i_, j_)].phi_bu) / cell_width;
	

    double grad_x, grad_y;

    if (nodes[id].phi_bu > 0) {
        grad_x = Max( PSquare(a), NSquare(b) );
        grad_y = Max( PSquare(c), NSquare(d) );
    } else {
        grad_x = Max( PSquare(b), NSquare(a) );
        grad_y = Max( PSquare(d), NSquare(c) );
    }

    return sqrt(grad_x + grad_y);
}

double LevelSet::Max(double a_, double b_)
{
	return a_ > b_ ? a_ : b_;
}

double LevelSet::Distance(double x_, double y_)
{
	return sqrt(pow((x_ - X0), 2) + pow((y_ - Y0), 2));
}
#endif