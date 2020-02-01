
#include "MainWindow.h"


//Construtor da classe
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	this->Debug = true; //valor do booleano debug
	if (Debug) qDebug() << "MainWindow::MainWindow()" << endl;

	this->ui = new Ui::MainWindow;
	this->ui->setupUi(this);
	this->setWindowTitle("VTK File Reader With Qt");
	//this->setWindowState(Qt::WindowMaximized); //Abre a view com a janela maximizada

	this->setup_qt(); // instancia combobox e a adiciona na toolbar
	this->setup_vtk(); //Instancia os objetos vtk declarados anteriormente no header
	this->connects(); //Faz os connects com os elementos da ui


}



//Destrutor da classe
MainWindow::~MainWindow()
{
	qApp->quit();
}


void MainWindow::setup_qt()
{
	//void QComboBox::addItem(const QString &text, const QVariant &userData = QVariant())
	list << "" << "Points" << "Surface" << "Surface With Edges" << "Wireframe";
	this->comboBox = new QComboBox;
	this->comboBox->addItems(list);
	//this->ui->toolBar->addWidget(this->comboBox);
	this->ui->toolBar->insertWidget(this->ui->actionShowMsg, this->comboBox); //insere combobox antes da action showmsg

}


void MainWindow::wireframe(QString text)
{
	if (text == "Wireframe") {
		this->actor->GetProperty()->SetRepresentationToWireframe();
		this->actor->GetProperty()->EdgeVisibilityOff();
		this->ui->qvtkwidget->update();
	}
	else if (text == "Surface") {
		this->actor->GetProperty()->SetRepresentationToSurface();
		this->actor->GetProperty()->EdgeVisibilityOff();
		this->ui->qvtkwidget->update();
	}
	else if (text == "Points") {
		this->actor->GetProperty()->SetRepresentationToPoints();
		this->actor->GetProperty()->EdgeVisibilityOff();
		this->ui->qvtkwidget->update();
	}
	else if(text == "Surface With Edges"){
		this->actor->GetProperty()->SetRepresentationToSurface();
		this->actor->GetProperty()->EdgeVisibilityOn();
		this->ui->qvtkwidget->update();
	}

}


//Instancia dos objetos vtk
void MainWindow::setup_vtk()
{
	if (Debug) qDebug() << "MainWindow::setup_vtk()" << endl;

	//draw file
	this->reader = vtkSmartPointer<vtkUnstructuredGridReader>::New(); //Leitor do unstructured grid
	this->geometryFilter = vtkSmartPointer<vtkGeometryFilter>::New(); //Filtro para transformar ug em polydata
	this->actor = vtkSmartPointer<vtkActor>::New(); //Ator
	this->mapper = vtkSmartPointer<vtkPolyDataMapper>::New(); //Mapper
	this->transform = vtkSmartPointer<vtkTransform>::New(); //Transform do ator
	this->renderer = vtkSmartPointer<vtkRenderer>::New(); //Renderer
	this->centerOfMass = vtkSmartPointer<vtkCenterOfMass>::New();
	this->triangleFilter = vtkSmartPointer<vtkTriangleFilter>::New();
	this->renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	//this->style = vtkSmartPointer<MouseInteractorStyle>::New();

	//show msg
	this->text = vtkSmartPointer<vtkVectorText>::New();
	this->elevation = vtkSmartPointer<vtkElevationFilter>::New();

	//axis
	this->axes = vtkSmartPointer<vtkAxesActor>::New(); //Axes actor
	this->orimarker = vtkSmartPointer<vtkOrientationMarkerWidget>::New(); // Orientation marker

	//camera
	this->camera = vtkSmartPointer<vtkCamera>::New();

	//cutter plane
	this->plane = vtkSmartPointer<vtkPlane>::New();
	this->cutter = vtkSmartPointer<vtkCutter>::New();
	this->cutterMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	this->planeActor = vtkSmartPointer<vtkActor>::New();

}



//Fit to object
void MainWindow::slotFit()
{
	if (this->renderer->GetRenderWindow()) {
		this->camera = vtkSmartPointer<vtkCamera>::New();
		this->camera = this->renderer->GetActiveCamera();
		this->camera->SetFocalPoint(0, 0, (bounds[4] + bounds[5]) / 2);
		this->camera->SetViewUp(-1, -1, 1);
		this->camera->SetPosition(0, 0, 1);
		this->renderer->ResetCamera();
		this->renderer->GetRenderWindow()->Render();
		this->ui->qvtkwidget->update();
	}
	else return;
}






// Abre arquivo vtk
void MainWindow::slotOpenFile()
{
	if(Debug) qDebug() << "MainWindow::slotOpenFile()" << endl;
	
	QString filename;
	filename = QFileDialog::getOpenFileName(this, tr("Choose a .vtk file:"), QDir::currentPath(), tr("VTK File (*.vtk)"));

	if (filename.isEmpty()) {
		qDebug() << "Nenhum arquivo selecionado" << endl;
		msg.setWindowTitle("Erro ao abrir arquivo");
		msg.setText("Nenhum arquivo selecionado");
		msg.exec();
		return;
	}

	//else
	this->draw_file(filename);
}






// Connect dos elementos da ui com os métodos (slot)
void MainWindow::connects()
{
	if (Debug) qDebug() << "MainWindow::connects()" << endl;

	QObject::connect(this->ui->actionOpenFile, SIGNAL(triggered()), this, SLOT(slotOpenFile()));
	QObject::connect(this->ui->actionShowMsg, SIGNAL(triggered()), this, SLOT(slotShowMsg()));
	QObject::connect(this->ui->actionCutterPlane, SIGNAL(triggered()), this, SLOT(slotCutterPlane()));
	QObject::connect(this->ui->actionFit, SIGNAL(triggered()), this, SLOT(slotFit()));
	QObject::connect(this->ui->actionSobre, SIGNAL(triggered()), this, SLOT(slotAbout()));


	//Slots para definir visualização
	QObject::connect(this->ui->actionSetPlusX, SIGNAL(triggered()), this, SLOT(setPlusX()));
	QObject::connect(this->ui->actionSetMinusX, SIGNAL(triggered()), this, SLOT(setMinusX()));
	QObject::connect(this->ui->actionSetPlusY, SIGNAL(triggered()), this, SLOT(setPlusY()));
	QObject::connect(this->ui->actionSetMinusY, SIGNAL(triggered()), this, SLOT(setMinusY()));
	QObject::connect(this->ui->actionSetPlusZ, SIGNAL(triggered()), this, SLOT(setPlusZ()));
	QObject::connect(this->ui->actionSetMinusZ, SIGNAL(triggered()), this, SLOT(setMinusZ()));

	
	// comboBox
	QObject::connect(comboBox, QOverload<const QString &>::of(&QComboBox::currentIndexChanged), [=](const QString &text) { this->wireframe(text); });

}







//Renderiza arquivo vtk no qvtkwidget
void MainWindow::draw_file(QString filename)
{
	if (Debug) {
		qDebug() << "MainWindow::draw_file()" << endl;
		qDebug() << "Renderizando arquivo:" << filename << endl;
	}

	//removendo actor antigo da renderer
	this->mapper->RemoveAllObservers();
	this->renderer->RemoveActor(actor);

	//leitura do arquivo vtk (source)
	this->reader->SetFileName(filename.toStdString().c_str());
	this->reader->Update();

	//filtro para converter o unstructured grid em polydata
	this->geometryFilter->SetInputConnection(reader->GetOutputPort());
	this->geometryFilter->Update();

	//triangle filter
	//this->triangleFilter->SetInputConnection(geometryFilter->GetOutputPort());
	//this->triangleFilter->Update();

	//adicionando o filtro na mapper
	this->mapper->ImmediateModeRenderingOn(); //legacy function, procurar alternativa
	this->mapper->SetInputConnection(geometryFilter->GetOutputPort());

	//adicionando o actor e transform
	this->transform->PostMultiply();
	this->transform->Translate(0,0,0);
	this->transform->Scale(1.0, 1.0, 1.0);
	this->actor->SetOrigin(0,0,0);
	this->actor->SetMapper(mapper);
	this->actor->SetUserTransform(transform);
	this->actor->GetProperty()->SetColor(1, 1, 1);

	//capturando limites do arquivo vtk
	this->actor->GetBounds(this->bounds);
	if (Debug) {
		qDebug() << "Limites:" << endl;
		qDebug() << " - Xmin:" << bounds[0] << endl;
		qDebug() << " - Xmax:" << bounds[1] << endl;
		qDebug() << " - Ymin:" << bounds[2] << endl;
		qDebug() << " - Ymax:" << bounds[3] << endl;
		qDebug() << " - Zmin:" << bounds[4] << endl;
		qDebug() << " - Zmax:" << bounds[5] << endl;
	}

	//adicionando actor na renderer
	this->renderer->AddActor(this->actor);
	this->renderer->SetBackground(0, 0, 0); //background

	//Calculando centro de massa do objeto renderizado
	polydata = geometryFilter->GetOutput(); //polydata para efetuar o calculo
	centerOfMass->SetInputData(polydata);
	centerOfMass->SetUseScalarsAsWeights(false); //determina se os pontos tem peso
	centerOfMass->Update();
	centerOfMass->GetCenter(center); //salva as coordenadas no array
	if (Debug) {
		qDebug() << "Centro de Massa:" << endl;
		qDebug() << " - x:" << center[0] << endl;
		qDebug() << " - y:" << center[1] << endl;
		qDebug() << " - z:" << center[2] << endl;
	}


	//Foco e posição da camera
	this->renderer->GetActiveCamera()->GetPosition(position);
	if (Debug) {
		qDebug() << "Posicao da camera" << endl;
		qDebug() << " - x:" << position[0] << endl;
		qDebug() << " - y:" << position[1] << endl;
		qDebug() << " - z:" << position[2] << endl;
	}
	if (Debug) qDebug() << "Distancia entre posicao e foco da camera:" << this->renderer->GetActiveCamera()->GetDistance() << endl;
	

	//Numero de pontos do objeto
	if(Debug) qDebug() << "Numero de pontos:" << polydata->GetNumberOfPoints() << endl;

	
	//O interactor default da renderwindow nao e compativel com o QVTKWidget 
	//substituiçao pelo interactor do QVTKWidget
	this->ui->qvtkwidget->GetRenderWindow()->SetInteractor(this->ui->qvtkwidget->GetInteractor());
	this->ui->qvtkwidget->GetRenderWindow()->AddRenderer(this->renderer);
	this->draw_axis();
	this->ui->qvtkwidget->update();


}


void MainWindow::slotShowMsg()
{
	if(Debug) qDebug() << "MainWindow::slotShowMsg()" << endl;

	this->mapper->RemoveAllObservers();
	this->renderer->RemoveActor(actor);

	//geometria do vector-text
	this->text->SetText("VTK and Qt");
	this->elevation->SetInputConnection(text->GetOutputPort());
	this->elevation->SetLowPoint(0, 0, 0); //definindo o tamanho da mensagem a ser exibida
	this->elevation->SetHighPoint(10, 0, 0);

	//mapper
	this->mapper->ImmediateModeRenderingOn();
	this->mapper->SetInputConnection(elevation->GetOutputPort());

	//actor
	this->actor->SetMapper(mapper);


	//interactor and mousestyle
	//this->renderWindowInteractor = this->ui->qvtkwidget->GetInteractor();
	//this->renderWindowInteractor->SetRenderWindow(this->ui->qvtkwidget->GetRenderWindow());
	//this->renderWindowInteractor->Initialize();
	//this->style->SetDefaultRenderer(renderer);
	//this->style->Data = triangleFilter->GetOutput();
	//this->renderWindowInteractor->SetInteractorStyle(style);


	//renderer
	this->renderer->AddActor(actor);
	this->renderer->ResetCamera();

	//jogando renderer para o qvtkwidget
	this->ui->qvtkwidget->GetRenderWindow()->AddRenderer(renderer);
	this->draw_axis();
	this->ui->qvtkwidget->update();

}



void MainWindow::setPlusX()
{
	if (this->renderer->GetRenderWindow()) {
		if (Debug) qDebug() << "MainWindow::SetPlusX()" << endl;
		double mediax = (bounds[0] + bounds[1]) / 2;
		this->camera = vtkSmartPointer<vtkCamera>::New();
		camera = this->renderer->GetActiveCamera();
		camera->SetViewUp(0, 0, 1);
		camera->SetFocalPoint(-mediax, 0, 0);
		camera->SetPosition(1, 0, 0);
		this->renderer->ResetCamera();
		this->renderer->GetRenderWindow()->Render();
	}
	else return;
}

void MainWindow::setMinusX()
{
	if (this->renderer->GetRenderWindow()) {
		if (Debug) qDebug() << "MainWindow::SetMinusX()" << endl;
		double mediax = (bounds[0] + bounds[1]) / 2;
		this->camera = vtkSmartPointer<vtkCamera>::New();
		this->camera = this->renderer->GetActiveCamera();
		camera->SetViewUp(0, 0, 1);
		camera->SetFocalPoint(mediax, 0, 0);
		camera->SetPosition(-1, 0, 0);
		this->renderer->ResetCamera();
		this->renderer->GetRenderWindow()->Render();
	}
	else return;
}

void MainWindow::setPlusY()
{
	if (this->renderer->GetRenderWindow()) {
		if (Debug) qDebug() << "MainWindow::SetPlusY()" << endl;
		double mediay = (bounds[2] + bounds[3]) / 2;
		this->camera = vtkSmartPointer<vtkCamera>::New();
		camera = this->renderer->GetActiveCamera();
		camera->SetViewUp(0, 0, 1);
		camera->SetFocalPoint(0, -mediay, 0);
		camera->SetPosition(0, 1, 0);
		this->renderer->ResetCamera();
		this->renderer->GetRenderWindow()->Render();
	}
	else return;
}


void MainWindow::setMinusY()
{
	if (this->renderer->GetRenderWindow()) {
		if (Debug) qDebug() << "MainWindow::setMinusY()" << endl;
		double mediay = (bounds[2] + bounds[3]) / 2;
		this->camera = vtkSmartPointer<vtkCamera>::New();
		camera = this->renderer->GetActiveCamera();
		camera->SetViewUp(0, 0, 1); //Coordenada ortogonal ao plano de visualização
		camera->SetFocalPoint(0, mediay, 0); //definindo ponto focal da camera
		camera->SetPosition(0, -1, 0); // posicionamento da camera no eixo desejado
		this->renderer->ResetCamera();
		this->renderer->GetRenderWindow()->Render();
	}
	else return;
}


void MainWindow::setPlusZ()
{
	if (this->renderer->GetRenderWindow()) {
		if (Debug) qDebug() << "MainWindow::setPlusZ()" << endl;
		double mediaz = (bounds[4] + bounds[5]) / 2;
		this->camera = vtkSmartPointer<vtkCamera>::New();
		camera = this->renderer->GetActiveCamera();
		camera->SetViewUp(1, 0, 0);
		camera->SetFocalPoint(0, 0, -mediaz);
		camera->SetPosition(0, 0, 1);
		this->renderer->ResetCamera();
		this->renderer->GetRenderWindow()->Render();
	}
	else return;
}


void MainWindow::setMinusZ()
{
	if (this->renderer->GetRenderWindow()) {
		if (Debug) qDebug() << "MainWindow::SetMinusZ()" << endl;
		double mediaz = (bounds[4] + bounds[5]) / 2;
		this->camera = vtkSmartPointer<vtkCamera>::New();
		camera = this->renderer->GetActiveCamera();
		camera->SetViewUp(1, 0, 0);
		camera->SetFocalPoint(0, 0, mediaz);
		camera->SetPosition(0, 0, -1);
		this->renderer->ResetCamera();
		this->renderer->GetRenderWindow()->Render();
	}
	else return;
}







//define plano de corte da malha
void MainWindow::slotCutterPlane()
{
	if (this->geometryFilter->GetOutputPort()) {
		if (Debug) qDebug() << "MainWindow::slotCutterPlane()" << endl;

		//this->mapper->RemoveAllObservers();
		this->renderer->RemoveActor(actor); // remove ator para deixar somente o plano de corte (slice)

		//this->planeSource->SetCenter(0.0, 0.0, 0.0); //metodo apenas para classe vtkPlaneSource

		// funcao implicita para corte (neste caso um plano)
		this->plane->SetOrigin(center);

		//plano de corte ao longo dos eixos [ xz = 1 0 0 / xy = 0 0 1 / yz = 0 1 0 ]
		this->plane->SetNormal(0, 0, 1); //vetor normal ao plano

		// cutter, mapper e actor
		this->cutter->SetCutFunction(plane); //funcao implicita para corte
		this->cutter->SetInputConnection(geometryFilter->GetOutputPort());
		this->cutter->Update();
		this->cutterMapper->SetInputConnection(cutter->GetOutputPort());
		this->planeActor->SetMapper(cutterMapper);

		// obtendo pontos do cutter
		qDebug() << "Pontos da borda:" << endl;
		int numberOfPoints = 0;
		for (int i = 0; i < cutter->GetOutput()->GetNumberOfPoints(); i++) {
			numberOfPoints++;
			double p[3] = { 0, 0, 0 };
			cutter->GetOutput()->GetPoint(i, p);
			qDebug() << p[0] << p[1] << p[2];
		}
		qDebug() << "Points" << numberOfPoints << endl;

		// adicionando planeActor na renderer
		this->renderer->AddActor(planeActor);

		this->draw_axis();
		this->ui->qvtkwidget->update();
	}
	else return;
}



//Renderiza os axis no canto inferior esquerdo da tela
void MainWindow::draw_axis()
{
	if (Debug) qDebug() << "MainWindow::draw_axis()" << endl;

	//tipo, tamanho e labels do axis
	this->axes->SetShaftTypeToLine();
	this->axes->SetTotalLength(1.0, 1.0, 1.0);
	this->axes->SetXAxisLabelText("x");
	this->axes->SetYAxisLabelText("y");
	this->axes->SetZAxisLabelText("z");
	//this->axes->SetNormalizedLabelPosition(.1, .1, .1); //posição normalizada das labels
	//this->axes->AxisLabelsOff(); //desativa labels do axis

	/* Calcula valor maximo dos limites
	double maximo;
	for (int i = 1; i < (sizeof(bounds) / sizeof(bounds[0])) / 2 - 1; i += 2) {
		maximo = std::max(bounds[i] - bounds[i - 1], bounds[i + 2] - bounds[i + 1]);
	}
	*/

	//posicionamento do axis no canto inferior da renderer
	this->orimarker->SetOutlineColor(0.9300, 0.5700, 0.1300);
	this->orimarker->SetOrientationMarker(this->axes);
	this->orimarker->SetInteractor(this->ui->qvtkwidget->GetRenderWindow()->GetInteractor());
	this->orimarker->SetViewport(0.0, 0.0, 0.3, 0.3);
	this->orimarker->SetEnabled(1);
	this->orimarker->InteractiveOff(); //desativa interação/movimentação do axis

	//atualiza view
	this->renderer->ResetCamera();
	this->ui->qvtkwidget->update();

}


// about section
void MainWindow::slotAbout()
{
	if (Debug) qDebug() << "MainWindow::slotAbout()" << endl;
	QMessageBox::information(this, "About", "by Gabriel Christo");

}
