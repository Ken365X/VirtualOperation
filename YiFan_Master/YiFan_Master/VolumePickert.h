#pragma once
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkDataSetMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkIdTypeArray.h>
#include <vtkPolyData.h>
#include <vtkCellArray.h>
#include <vtkSphereSource.h>
#include <vtkCellPicker.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkProperty.h>
#include <vtkSelectionNode.h>
#include <vtkSelection.h>
#include <vtkExtractSelection.h>
#include <vtkObjectFactory.h>
#include <vtkVolume.h>
#include <vtkVolumePicker.h>
#include <iostream>
#include <vtkKdTreePointLocator.h>
#include <vtkOctreePointLocator.h>
#include <vtkCoordinate.h>
#include"head.h"
#include"vtkPixelManager.h"

class VolumePickerInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
	static VolumePickerInteractorStyle* New();

	VolumePickerInteractorStyle()
	{
		// selectedMapper = vtkSmartPointer<vtkDataSetMapper>::New();
		selectedVolume = vtkSmartPointer<vtkVolume>::New();
	}

	virtual void OnRightButtonDown()
	{
		// get the position of the event,and send it to picker
		int* pos = this->GetInteractor()->GetEventPosition();

		// volunmepicker get the world point
		vtkSmartPointer<vtkCellPicker> volumepicker = vtkSmartPointer<vtkCellPicker>::New();
		volumepicker->SetTolerance(0.0005);
		// the volumepicker could pick the clipping planes,but we think it is not necessary,so we disabled this function
		volumepicker->PickClippingPlanesOff();
		volumepicker->SetUseVolumeGradientOpacity(90);
		volumepicker->Pick(pos[0], 
			               pos[1], 
			               0, 
			               this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
		
		//cout << this->Interactor->GetRenderWindow()->GetRenderers()->GetItemAsObject(0) << endl;

		if (volumepicker->GetCellId() != -1)
		{
			std::cout << volumepicker->GetCellId() << std::endl;
			int ID = volumepicker->GetCellId();
			
			vtkSmartPointer<vtkKdTreePointLocator> kDTree = vtkSmartPointer<vtkKdTreePointLocator>::New();
			
			kDTree->SetDataSet(((vtkVolumeMapper *)((vtkVolume *)this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->GetVolumes()->GetItemAsObject(0))->GetMapper())->GetInput());
			
			kDTree->BuildLocator();
			double closestPoint[3];
			kDTree->GetDataSet()->GetPoint(ID, closestPoint);
			volumepicker->GetMapperPosition(closestPoint);
			std::cout << "Coordinates: " << closestPoint[0] << " " << closestPoint[1] << " " << closestPoint[2] << std::endl;


			//vtkSmartPointer<vtkOctreePointLocator> OctTree =
			//	vtkSmartPointer<vtkOctreePointLocator>::New();
			////kDTree->SetDataSet(selectedVolume->GetMapper()->GetDataSetInput);
			////cout << ((vtkVolume *)this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->GetVolumes()->GetItemAsObject(0))->GetMapper()->GetDataSetInput();
			//OctTree->SetDataSet(((vtkVolume *)this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->GetVolumes()->GetItemAsObject(0))->GetMapper()->GetDataSetInput());
			//OctTree->BuildLocator();
			//double closestPoint[3];
			//OctTree->GetDataSet()->GetPoint(ID, closestPoint);
			//std::cout << "Coordinates: " << closestPoint[0] << " " << closestPoint[1] << " " << closestPoint[2] << std::endl;

			double * point = closestPoint;
			vtkSmartPointer<vtkSphereSource> sphereSource =
				vtkSmartPointer<vtkSphereSource>::New();
			sphereSource->Update();

			vtkSmartPointer<vtkPolyDataMapper> mapper =
				vtkSmartPointer<vtkPolyDataMapper>::New();
			mapper->SetInputConnection(sphereSource->GetOutputPort());
			vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
			actor->SetMapper(mapper);
			actor->SetPosition(point);
			actor->SetScale(10);
			actor->GetProperty()->SetColor(1.0, 0.0, 0.0);
			this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actor);
			
		}
		vtkInteractorStyleTrackballCamera::OnRightButtonDown();
	}

	vtkSmartPointer<vtkVolume>        selectedVolume;
	// manager hasn't been used before, and OnRightButtonDown has no connection about *manager
	vtkPixelManager *manager;
};