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

	virtual void OnKeyDown()
	{
		if (GetInteractor()->GetKeyCode() == ' ')
		{
			if (isAltDown == true)
			{
				isAltDown = false;
				GetInteractor()->GetRenderWindow()->Render();
			}
			else
			{
				isAltDown = true;
			}
		}
		else if (GetInteractor()->GetKeyCode() == 'k')
		{
			if (boxWidget == nullptr)
			{
				boxWidget =
					vtkSmartPointer<vtkBoxWidget>::New();
				boxWidget->SetInteractor(this->Interactor);
				boxWidget->SetPlaceFactor(1);
				boxWidget->PlaceWidget(((vtkVolume *)(this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->GetVolumes()->GetItemAsObject(0)))->GetBounds());
				boxWidget->InsideOutOn();
				boxWidget->TranslationEnabledOff();
				boxWidget->PickingManagedOff();
				boxWidget->EnabledOff();
				boxWidget->RotationEnabledOff();
				boxWidget->SetKeyPressActivationValue('k');
				boxWidget->OutlineCursorWiresOff();
				boxWidget->OutlineFaceWiresOff();
				boxWidget->HandlesOff();
				boxWidget->On();
				return;
			}
		}
		else if(GetInteractor()->GetKeyCode() == 'l')
		{
			if (planeWidget == nullptr)
			{
				planeWidget = vtkSmartPointer<vtkImplicitPlaneWidget>::New();
				sliderWidget = vtkSmartPointer<vtkSliderWidget>::New();
				
				planeWidget->SetInteractor(this->Interactor);
				// planeWidget->SetRepresentation(implicitplaneRep);
				planeWidget->SetPlaceFactor(1);
				planeWidget->TubingOff();
				planeWidget->GetPlaneProperty()->SetColor(.96,.96,.96);
				planeWidget->GetPlaneProperty()->SetOpacity(.5);
				planeWidget->PlaceWidget(((vtkVolume *)(this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->GetVolumes()->GetItemAsObject(0)))->GetBounds());
				planeWidget->On();
				planeWidget->SetKeyPressActivationValue('l');

				vtkSmartPointer<vtkSliderRepresentation3D> rep = vtkSmartPointer<vtkSliderRepresentation3D>::New();
				rep->SetMaximumValue(15);
				rep->SetMinimumValue(1);
				rep->SetValue(10);
				rep->SetTitleText("Slice width");
				rep->SetTitleHeight(0.04);
				rep->SetSliderShapeToSphere();
				rep->GetPoint2Coordinate()->SetCoordinateSystemToWorld();
				rep->GetPoint2Coordinate()->SetValue(0, 0, 30);
				rep->GetPoint1Coordinate()->SetCoordinateSystemToWorld();
				rep->GetPoint1Coordinate()->SetValue(10, 10, 400);
				//double * AA = ((vtkVolume *)(this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->GetVolumes()->GetItemAsObject(0)))->GetBounds();
				double AA[6] = { 0,10,0,10,30 ,400};
				rep->PlaceWidget(AA);
				rep->SetPlaceFactor(1);
				sliderWidget->SetInteractor(this->Interactor);
				sliderWidget->SetAnimationModeToAnimate();
				sliderWidget->SetRepresentation(rep);
				sliderWidget->SetKeyPressActivationValue('h');
				sliderWidget->EnabledOn();
				sliderWidget->On();
				return;
			}
		}
		else if (GetInteractor()->GetKeyCode() == 'm')
		{
			if (planeWidget != nullptr)
			{
				double scla = 10;
				scla = sliderWidget->GetSliderRepresentation()->GetValue();
				vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
				planeWidget->GetPlane(plane);
				double * orgin = plane->GetOrigin();
				double * normal = plane->GetNormal();
				// cout << "origin: " << orgin[0] << ' ' << orgin[1] << ' ' << orgin[2] << endl;
				// cout << "normal: " << normal[0] << ' ' << normal[1] << ' ' << normal[2] << endl;
				// cout << normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2];
				vtkSmartPointer<vtkPlane> plane1 = plane->NewInstance();
				vtkSmartPointer<vtkPlane> plane2 = plane->NewInstance();
				double orgin1[3];
				double normal1[3];
				orgin1[0] = orgin[0] + scla * normal[0];
				orgin1[1] = orgin[1] + scla * normal[1];
				orgin1[2] = orgin[2] + scla * normal[2];

				normal1[0] = -normal[0];
				normal1[1] = -normal[1];
				normal1[2] = -normal[2];
				// cout << "origin1: " << orgin1[0] << ' ' << orgin1[1] << ' ' << orgin1[2] << endl;
				// cout << "normal: " << normal1[0] << ' ' << normal1[1] << ' ' << normal1[2] << endl;
				plane1->SetOrigin(orgin1);
				plane1->SetNormal(normal1);
				double orgin2[3];
				orgin2[0] = orgin[0] - scla * normal[0];
				orgin2[1] = orgin[1] - scla * normal[1];
				orgin2[2] = orgin[2] - scla * normal[2];
				// cout << "origin2: " << orgin2[0] << ' ' << orgin2[1] << ' ' << orgin2[2] << endl;
				// cout << "normal: " << normal[0] << ' ' << normal[1] << ' ' << normal[2] << endl;
				plane2->SetOrigin(orgin2);
				plane2->SetNormal(normal);

				((vtkVolumeMapper *)((vtkVolume *)this->Interactor->GetRenderWindow()->
					GetRenderers()->GetFirstRenderer()->GetVolumes()->GetItemAsObject(0))
					->GetMapper())->AddClippingPlane(plane1);

				((vtkVolumeMapper *)((vtkVolume *)this->Interactor->GetRenderWindow()->
					GetRenderers()->GetFirstRenderer()->GetVolumes()->GetItemAsObject(0))
					->GetMapper())->AddClippingPlane(plane2);
			}
		}
		else if (GetInteractor()->GetKeyCode() == 'n')
		{
			if (planeWidget != nullptr)
			{
				vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
				planeWidget->GetPlane(plane);

				((vtkVolumeMapper *)((vtkVolume *)this->Interactor->GetRenderWindow()->
					GetRenderers()->GetFirstRenderer()->GetVolumes()->GetItemAsObject(0))
					->GetMapper())->AddClippingPlane(plane);
			}
		}
		else
		{
			// VOID
		}
		vtkInteractorStyleTrackballCamera::OnKeyDown();
	}

	virtual void OnMouseMove()
	{
		if(!isAltDown)
		{
			vtkInteractorStyleTrackballCamera::OnMouseMove();
			return;
		}
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
		vtkInteractorStyleTrackballCamera::OnMouseMove();
	}

	bool isAltDown = false;
	vtkSmartPointer<vtkVolume>               selectedVolume;
	vtkSmartPointer<vtkBoxWidget>			 boxWidget;
	vtkSmartPointer<vtkImplicitPlaneWidget>  planeWidget;
	vtkSmartPointer<vtkSliderWidget>         sliderWidget;

	// manager hasn't been used before, and OnRightButtonDown has no connection about *manager
	vtkPixelManager *manager;
};