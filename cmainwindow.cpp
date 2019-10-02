#include "cmainwindow.h"
#include "ui_cmainwindow.h"


#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QAspectEngine>

#include <Qt3DRender/qrenderaspect.h>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QMaterial>

#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QTorusMesh>
#include <Qt3DExtras/QExtrudedTextMesh>

#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DExtras/QPhongMaterial>

#include <Qt3DExtras/QFirstPersonCameraController>

#include <Qt3DRender/QSceneLoader>


class SceneWalker : public QObject
{
public:
	SceneWalker(Qt3DRender::QSceneLoader *loader) : m_loader(loader) { }

	void onStatusChanged();

private:
	void walkEntity(Qt3DCore::QEntity *e, int depth = 0);

	Qt3DRender::QSceneLoader *m_loader;
};

void SceneWalker::onStatusChanged()
{
	qDebug() << "Status changed:" << m_loader->status();
	if (m_loader->status() != Qt3DRender::QSceneLoader::Ready)
		return;

	// The QSceneLoader instance is a component of an entity. The loaded scene
	// tree is added under this entity.
	QVector<Qt3DCore::QEntity *> entities = m_loader->entities();

	// Technically there could be multiple entities referencing the scene loader
	// but sharing is discouraged, and in our case there will be one anyhow.
	if (entities.isEmpty())
		return;
	Qt3DCore::QEntity *root = entities[0];
	// Print the tree.
	walkEntity(root);

	// To access a given node (like a named mesh in the scene), use QObject::findChild().
	// The scene structure and names always depend on the asset.
	Qt3DCore::QEntity *e = root->findChild<Qt3DCore::QEntity *>(QStringLiteral("PlanePropeller_mesh")); // toyplane.obj
	if (e)
		qDebug() << "Found propeller node" << e << "with components" << e->components();
}

void SceneWalker::walkEntity(Qt3DCore::QEntity *e, int depth)
{
	Qt3DCore::QNodeVector nodes = e->childNodes();
	for (int i = 0; i < nodes.count(); ++i) {
		Qt3DCore::QNode *node = nodes[i];
		Qt3DCore::QEntity *entity = qobject_cast<Qt3DCore::QEntity *>(node);
		if (entity) {
			QString indent;
			indent.fill(' ', depth * 2);
			qDebug().noquote() << indent << "Entity:" << entity << "Components:" << entity->components();
			walkEntity(entity, depth + 1);
		}
	}
}

cMainWindow::cMainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::cMainWindow)
{
	ui->setupUi(this);

	Qt3DExtras::Qt3DWindow*						view			= new Qt3DExtras::Qt3DWindow();

	// Scene Root
	Qt3DCore::QEntity*							sceneRoot		= new Qt3DCore::QEntity();

	// Scene Camera
	Qt3DRender::QCamera*						basicCamera		= view->camera();
	basicCamera->setProjectionType(Qt3DRender::QCameraLens::PerspectiveProjection);
	basicCamera->setUpVector(QVector3D(0.0f, 1.0f, 0.0f));
	basicCamera->setViewCenter(QVector3D(0.0f, 3.5f, 0.0f));
	basicCamera->setPosition(QVector3D(0.0f, 3.5f, 25.0f));

	// For camera controls
	Qt3DExtras::QFirstPersonCameraController*	camController	= new Qt3DExtras::QFirstPersonCameraController(sceneRoot);
	camController->setCamera(basicCamera);

	Qt3DCore::QEntity*			sceneLoaderEntity	= new Qt3DCore::QEntity(sceneRoot);
	Qt3DRender::QSceneLoader*	sceneLoader			= new Qt3DRender::QSceneLoader(sceneLoaderEntity);
	SceneWalker sceneWalker(sceneLoader);
	QObject::connect(sceneLoader, &Qt3DRender::QSceneLoader::statusChanged, &sceneWalker, &SceneWalker::onStatusChanged);
	sceneLoaderEntity->addComponent(sceneLoader);

	sceneLoader->setSource(QUrl::fromLocalFile("C:\\Users\\VET0572\\OneDrive - WINDESIGN\\Villa Kunterbunt\\OBJ\\New Folder.obj"));

	view->setRootEntity(sceneRoot);

	QWidget*	m_lp3DWidget = QWidget::createWindowContainer(view);
	ui->gridLayout->addWidget(m_lp3DWidget, 0, 0, 1, 1);
}

//cMainWindow::cMainWindow(QWidget *parent) :
//	QMainWindow(parent),
//	ui(new Ui::cMainWindow)
//{
//	ui->setupUi(this);

//	Qt3DExtras::Qt3DWindow* view	= new Qt3DExtras::Qt3DWindow();
//	Qt3DCore::QEntity* scene = createTestScene();

//	// camera
//	Qt3DRender::QCamera *camera = view->camera();
//	camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
//	camera->setPosition(QVector3D(0, 0, 40.0f));
//	camera->setViewCenter(QVector3D(0, 0, 0));

//	// manipulator
//	Qt3DExtras::QOrbitCameraController* manipulator = new Qt3DExtras::QOrbitCameraController(scene);
//	manipulator->setLinearSpeed(50.f);
//	manipulator->setLookSpeed(180.f);
//	manipulator->setCamera(camera);

//	view->setRootEntity(scene);


//	QWidget*	m_lp3DWidget = QWidget::createWindowContainer(view);
//	ui->gridLayout->addWidget(m_lp3DWidget, 0, 0, 1, 1);
//}

cMainWindow::~cMainWindow()
{
	delete ui;
}
