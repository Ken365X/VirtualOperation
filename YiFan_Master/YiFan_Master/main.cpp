#include"head.h"
#include"PointerPickerInteractorStyle.h"
#include"vtkPixelManager.h"
#include"VolumePickert.h"
vtkStandardNewMacro(PointerPickerInteractorStyle);
vtkStandardNewMacro(VolumePickerInteractorStyle);
///////////////////////////////////////////////////////////////////////////
#include<iostream>
using namespace std;
int main()
{
	VTK_MODULE_INIT(vtkInteractionStyle);
	VTK_MODULE_INIT(vtkRenderingOpenGL2);
	VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2);

	vtkSmartPointer<vtkBMPReader> reader = vtkSmartPointer<vtkBMPReader>::New();
	vtkSmartPointer<vtkImageAppend> append = vtkSmartPointer<vtkImageAppend>::New();
	append->SetAppendAxis(2);
	vtkSmartPointer<vtkStringArray > fileArray = vtkSmartPointer<vtkStringArray >::New();
	char fileName[128];
	for (int i = 1; i <= 200; i++)
	{
		sprintf(fileName, "C:\\Users\\27902\\Desktop\\VHP\\a_vm.%03d.bmp", i);
		std::string fileStr(fileName);
		fileArray->InsertNextValue(fileStr);
	}
	reader->SetFileNames(fileArray);
	reader->Update();

	vtkSmartPointer<vtkImageLuminance> Luminance = vtkSmartPointer<vtkImageLuminance>::New();
	Luminance->SetInputData(reader->GetOutput());
	Luminance->Update();

	append->SetInputData(reader->GetOutput());
	append->Update();

	int dims[3];
	append->GetOutput()->GetDimensions(dims);
	std::cout << "dim:" << dims[0] << "  " << dims[1] << "  " << dims[2] << endl;
	double spaces[3];
	append->GetOutput()->GetSpacing(spaces);
	std::cout << "space:" << "  " << spaces[0] << "  " << spaces[1] << "  " << spaces[2] << endl;

	vtkSmartPointer<vtkImageCast> cast = vtkSmartPointer<vtkImageCast>::New();
	cast->SetInputData(Luminance->GetOutput());
	cast->SetOutputScalarTypeToUnsignedShort();
	cast->Update();

	/*vtkSmartPointer<vtkImageData>  source = vtkSmartPointer<vtkImageData>::New();
	source->SetDimensions(100, 100, 100);
	source->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
	int dim[3];
	source->GetDimensions(dim);
	for (int k = 0; k < dim[2]; k++)
	{
	for (int j = 0; j < dim[1]; j++)
	{
	for (int i = 0; i < dim[0]; i++)
	{
	unsigned char *pixel = (unsigned char *)source->GetScalarPointer(i, j, k);
	*pixel = 125;
	}
	}
	}*/  //测试用的正方形
	vtkSmartPointer<vtkVolumeRayCastCompositeFunction> fun = vtkSmartPointer<vtkVolumeRayCastCompositeFunction>::New();
	vtkSmartPointer<vtkVolumeRayCastMIPFunction> fun1 = vtkSmartPointer<vtkVolumeRayCastMIPFunction>::New();
	fun1->SetMaximizeMethodToOpacity();
	vtkSmartPointer<vtkVolumeRayCastMapper>  Mapper = vtkSmartPointer<vtkVolumeRayCastMapper>::New();
	vtkSmartPointer<vtkVolumeProperty>  property = vtkSmartPointer<vtkVolumeProperty>::New();
	vtkSmartPointer<vtkColorTransferFunction> color = vtkSmartPointer<vtkColorTransferFunction>::New();
	color->AddRGBPoint(0.00, 0.00, 0.00, 0.00);
	color->AddRGBPoint(64.00, 1.00, 0.52, 0.30);
	color->AddRGBPoint(190.0, 1.00, 1.00, 1.00);
	color->AddRGBPoint(220, 0.20, 0.20, 0.20);
	property->SetColor(color);
	property->SetInterpolationTypeToLinear();
	property->ShadeOn();
	property->SetAmbient(0.4);
	property->SetDiffuse(0.6);
	property->SetSpecular(0.2);
	Mapper->SetInputData(cast->GetOutput());
	Mapper->SetVolumeRayCastFunction(fun);
	vtkSmartPointer<vtkVolume>  volume = vtkSmartPointer<vtkVolume>::New();
	volume->SetProperty(property);
	volume->SetMapper(Mapper);
	volume->Update();
	vtkSmartPointer<vtkPiecewiseFunction> compositeOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
	compositeOpacity->AddPoint(90, 0.00);
	compositeOpacity->AddPoint(100, 0.40);
	compositeOpacity->AddPoint(180, 0.60);
	property->SetScalarOpacity(compositeOpacity);

	vtkPixelManager manager;
	manager.SetInputData(cast->GetOutput());
	// 傅川溥修改
	//	manager.SetInputData(source);
	manager.initial();
	manager.Update();
	vtkSmartPointer<vtkRenderer>  render = vtkSmartPointer<vtkRenderer>::New();
	render->AddVolume(volume);
	render->SetBackground(1, 1, 1);
	vtkSmartPointer<vtkRenderWindow>  window = vtkSmartPointer<vtkRenderWindow>::New();
	window->AddRenderer(render);
	window->Render();
	vtkSmartPointer<vtkPointPicker> PointPicker = vtkSmartPointer<vtkPointPicker>::New();
	vtkSmartPointer<vtkVolumePicker> volumepicker = vtkSmartPointer<vtkVolumePicker>::New();

	// 傅川溥修改，更换了一个交互方式
	vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	window->SetInteractor(interactor);
	//vtkSmartPointer<PointerPickerInteractorStyle> style = vtkSmartPointer<PointerPickerInteractorStyle>::New();
	//style->SetDefaultRenderer(render);
	vtkSmartPointer<VolumePickerInteractorStyle> volumestyle = vtkSmartPointer<VolumePickerInteractorStyle>::New();
	volumestyle->SetDefaultRenderer(render);
	//interactor->SetInteractorStyle(style);
	//interactor->SetPicker(PointPicker);
	interactor->SetInteractorStyle(volumestyle);
	interactor->SetPicker(volumepicker);
	//style->img = cast->GetOutput();
	volumestyle->manager = &manager;

	interactor->SetRenderWindow(window);
	interactor->Initialize();
	interactor->Start();
	return 0;
}