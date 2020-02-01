#pragma once

// Includes c/c++
#include <cstdlib>
#include <algorithm>

//#include <MouseInteractorStyle.h>

//Includes Qt
#include <QMainWindow>
#include <QFileDialog.h>
#include <QDir.h>
#include <QDebug.h>
#include <QMessageBox.h>
#include <qcombobox.h>


//Includes vtk
#include <vtkSmartPointer.h> 
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkElevationFilter.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkGeometryFilter.h>
#include <vtkVectorText.h>
#include <vtkTransform.h>
#include <vtkCamera.h>
#include <vtkAxesActor.h>
#include <vtkCenterOfMass.h>
#include <vtkPolyData.h>
#include <vtkPlane.h>
#include <vtkCutter.h>
#include<vtkTriangleFilter.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>

//Include da ui
#include "ui_MainWindow.h"


//extende a classe mainwindow do qt
class MainWindow : public QMainWindow
{
	Q_OBJECT
	//macro necessaria para o meta object compiler (moc) do qt
	//ira completar codigo c++ no header, como o include da ui por exemplo
	//necessario para classes que usam o mecanismo signal/slot

public:
	MainWindow(QWidget *parent = Q_NULLPTR);
	~MainWindow();

	void connects();
	void draw_file(QString filename);
	void setup_vtk();
	void draw_axis();
	

public slots:

	void slotOpenFile();
	void slotShowMsg();
	void slotCutterPlane();
	void slotFit();
	void slotAbout();
	void setup_qt();
	void wireframe(QString text);
	
	void setPlusX();
	void setMinusX();
	void setPlusY();
	void setMinusY();
	void setPlusZ();
	void setMinusZ();

private:

	//UI file para classe MainWindow
	Ui::MainWindow *ui;

	//Qt
	QMessageBox msg;
	QComboBox *comboBox;
	QStringList list;
	
	// show message
	vtkSmartPointer<vtkVectorText> text;
	vtkSmartPointer<vtkElevationFilter> elevation;

	//draw file
	vtkSmartPointer<vtkGeometryFilter> geometryFilter;
	vtkSmartPointer<vtkUnstructuredGridReader> reader;
	vtkSmartPointer<vtkPolyDataMapper> mapper;
	vtkSmartPointer<vtkActor> actor;
	vtkSmartPointer<vtkTransform> transform;
	vtkSmartPointer<vtkRenderer> renderer;
	vtkSmartPointer<vtkCenterOfMass> centerOfMass;
	vtkPolyData* polydata;
	vtkSmartPointer<vtkTriangleFilter> triangleFilter;
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
	//vtkSmartPointer<MouseInteractorStyle> style;

	//camera
	vtkSmartPointer<vtkCamera> camera;

	//cutter plane
	vtkSmartPointer<vtkPlane> plane;
	vtkSmartPointer<vtkCutter> cutter;
	vtkSmartPointer<vtkPolyDataMapper> cutterMapper;
	vtkSmartPointer<vtkActor> planeActor;

	//axis
	vtkSmartPointer<vtkAxesActor> axes;
	vtkSmartPointer<vtkOrientationMarkerWidget> orimarker;

	//Variavel para controlar debug
	bool Debug;

	//Array de double para guardar extremos de um dado arquivo de pontos
	double bounds[6];

	//Array de double para guardar as coordenadas do centro de massa
	double center[3];

	//Array para guardar camera position
	double position[3];

};
