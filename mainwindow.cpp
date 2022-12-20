#include "mainwindow.h"

#include "initialization/meshinitializer.h"
#include "initialization/objfile.h"
#include "subdivision/catmullclarksubdivider.h"
#include "subdivision/subdivider.h"
#include "ui_mainwindow.h"

/**
 * @brief MainWindow::MainWindow Creates a new Main Window UI.
 * @param parent Qt parent widget.
 */
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {

    ui->setupUi(this);
    ui->MeshGroupBox->setEnabled(ui->MainDisplay->settings.modelLoaded);
    ui->renderSettingsGroupBox->setEnabled(ui->MainDisplay->settings.modelLoaded);
    ui->tessSettingsGB->setVisible(
    ui->MainDisplay->settings.tesselationMode);
}

/**
 * @brief MainWindow::~MainWindow Deconstructs the main window.
 */
MainWindow::~MainWindow() {
    delete ui;
    meshes.clear();
    meshes.squeeze();
}

/**
 * @brief MainWindow::importOBJ Imports an obj file and adds the constructed
 * half-edge to the collection of meshes.
 * @param fileName Path of the .obj file.
 */
void MainWindow::importOBJ(const QString& fileName) {
    OBJFile newModel = OBJFile(fileName);
    meshes.clear();
    meshes.squeeze();

    if (newModel.loadedSuccessfully()) {
        MeshInitializer meshInitializer;
        meshes.append(meshInitializer.constructHalfEdgeMesh(newModel));
        ui->MainDisplay->updateBuffers(meshes[0]);
        ui->TessellationCheckBox->setChecked(false);
        ui->limitProjectioncheckBox->setChecked(false);
        ui->HideMeshCheckBox->setChecked(false);
        ui->tessTypecomboBox->setCurrentIndex(0);

        ui->MainDisplay->settings.modelLoaded = true;
    } else {
        ui->MainDisplay->settings.modelLoaded = false;
    }

    ui->MeshGroupBox->setEnabled(ui->MainDisplay->settings.modelLoaded);
    ui->renderSettingsGroupBox->setEnabled(ui->MainDisplay->settings.modelLoaded);
    ui->SubdivSteps->setValue(0);

    ui->outerTessLevel0->setValue(4);
    ui->outerTessLevel1->setValue(4);
    ui->outerTessLevel2->setValue(4);
    ui->outerTessLevel3->setValue(4);

    ui->innerTessLevel0->setValue(4);
    ui->innerTessLevel1->setValue(4);

    ui->MainDisplay->update();
}

// Don't worry about adding documentation for the UI-related functions.

void MainWindow::on_LoadOBJ_pressed() {
    QString filename = QFileDialog::getOpenFileName(
      this, "Import OBJ File", "../", tr("Obj Files (*.obj)"));
    importOBJ(filename);
}

void MainWindow::on_MeshPresetComboBox_currentTextChanged(
    const QString& meshName) {
    importOBJ(":/models/" + meshName + ".obj");
}

void MainWindow::on_SubdivSteps_valueChanged(int value) {
    Subdivider* subdivider = new CatmullClarkSubdivider();
    for (int k = meshes.size() - 1; k < value; k++) {
        meshes.append(subdivider->subdivide(meshes[k]));
    }
    ui->MainDisplay->updateBuffers(meshes[value]);
    delete subdivider;
    if(ui->MainDisplay->settings.showLimitProjection){
        on_limitProjectioncheckBox_toggled(true);
    }
}

void MainWindow::on_TessellationCheckBox_toggled(bool checked) {
    ui->tessSettingsGB->setVisible(checked);
    ui->MainDisplay->settings.tesselationMode = checked;
    ui->MainDisplay->settings.uniformUpdateRequired = true;
    ui->MainDisplay->update();
}

void MainWindow::on_HideMeshCheckBox_toggled(bool checked) {
    // Useful for clearly seeing only the patches rendered by the Tessellation
    // shaders.
    ui->MainDisplay->settings.showCpuMesh = !checked;
    ui->MainDisplay->settings.uniformUpdateRequired = true;
    ui->MainDisplay->update();
}

void MainWindow::on_tessTypecomboBox_currentTextChanged(const QString &tessType)
{
    if (tessType == "All patches"){
        ui->MainDisplay->settings.showAllPatchTessellation = true;
        ui->MainDisplay->settings.showOnlyRegularTessellation = false;
    }
    else{
        ui->MainDisplay->settings.showAllPatchTessellation = false;
        ui->MainDisplay->settings.showOnlyRegularTessellation = true;
    }
    ui->MainDisplay->settings.uniformUpdateRequired = true;
    ui->MainDisplay->update();
}

void MainWindow::on_outerTessLevel0_valueChanged(int val)
{
    ui->MainDisplay->settings.outerTessLevel0 = val;
    ui->MainDisplay->settings.uniformUpdateRequired = true;
    ui->MainDisplay->update();
}


void MainWindow::on_outerTessLevel1_valueChanged(int val)
{
    ui->MainDisplay->settings.outerTessLevel1 = val;
    ui->MainDisplay->settings.uniformUpdateRequired = true;
    ui->MainDisplay->update();
}


void MainWindow::on_outerTessLevel2_valueChanged(int val)
{
    ui->MainDisplay->settings.outerTessLevel2 = val;
    ui->MainDisplay->settings.uniformUpdateRequired = true;
    ui->MainDisplay->update();
}


void MainWindow::on_outerTessLevel3_valueChanged(int val)
{
    ui->MainDisplay->settings.outerTessLevel3 = val;
    ui->MainDisplay->settings.uniformUpdateRequired = true;
    ui->MainDisplay->update();
}


void MainWindow::on_innerTessLevel0_valueChanged(int val)
{
    ui->MainDisplay->settings.innerTessLevel0 = val;
    ui->MainDisplay->settings.uniformUpdateRequired = true;
    ui->MainDisplay->update();
}

void MainWindow::on_innerTessLevel1_valueChanged(int val)
{
    ui->MainDisplay->settings.innerTessLevel1 = val;
    ui->MainDisplay->settings.uniformUpdateRequired = true;
    ui->MainDisplay->update();
}

void MainWindow::on_limitProjectioncheckBox_toggled(bool checked)
{
    ui->MainDisplay->settings.showLimitProjection = checked;
    if (checked){
        Subdivider* subdivider = new LimitPositionSubdivider();
        limitProjectionMesh = subdivider->subdivide(meshes[ui->SubdivSteps->value()]);
        ui->MainDisplay->updateBuffers(limitProjectionMesh);
        delete subdivider;
    }
    else{
        ui->MainDisplay->updateBuffers(meshes[ui->SubdivSteps->value()]);
    }
    ui->MainDisplay->update();

}
// Trivial functions
void MainWindow::on_outerTessLevel_valueChanged(int val) {

}
void MainWindow::on_spinBox_2_valueChanged(int val) {

}



