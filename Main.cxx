
#include "MainWindow.h"
#include <QApplication>

/*
ToDo

* axis proporcionais
* plano de corte / clipfilter
* seleção nodal/pontos
* algoritmo de obtenção da superficie
* separar colorir objetos no vtk
* slice -> apenas plano
* clip -> sentido + do plano
* exemplo cone 6 + tdpflex


Classes To Look
* boxwidget
* vtkpoints (data)
* interactor

a,b = reader.GetOutput().GetScalarRange()
W,H,D = reader.GetOutput().GetDimensions()
lut = vtkLookupTable()
lut.SetHueRange(0.667, 0.0)
lut.SetTableRange(a,b)


toTable->SetFieldType(vtkDataObjectToTable::POINT_DATA)
renderwindowinteractor FlyTo (vtkRenderer *ren, double x, double y, double z)
https://computergraphics.stackexchange.com
http://sese.nu/scientific-visualisation-schedule-and-content-ht15/
https://en.wikipedia.org/wiki/Transformation_matrix
https://gamedev.stackexchange.com/questions/72044/why-do-we-use-4x4-matrices-to-transform-things-in-3d
https://github.com/martijnkoopman/Qt-VTK-viewer


Fundamental algortihms:
! Colour Mapping
! Streamlines
! Cut Planes
! Glyphs
! Contouring
! Marching Cubes
! Hedgehogs

*/


int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	app.setApplicationName("Qt5 Sandbox");
	MainWindow mainWindow;
	mainWindow.show();
	return app.exec();
}
