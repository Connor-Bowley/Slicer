/*=========================================================================

 Copyright (c) ProxSim ltd., Kwun Tong, Hong Kong. All Rights Reserved.

 See COPYRIGHT.txt
 or http://www.slicer.org/copyright/copyright.txt for details.

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 This file was originally developed by Davide Punzo, punzodavide@hotmail.it,
 and development was supported by ProxSim ltd.

=========================================================================*/

/**
 * @class   vtkSlicerMarkupsWidgetRepresentation3D
 * @brief   Default representation for the markups widget in 3D views
 *
 * @sa
 * vtkSlicerMarkupsWidgetRepresentation vtkSlicerMarkupsWidget
*/

#ifndef vtkSlicerMarkupsWidgetRepresentation3D_h
#define vtkSlicerMarkupsWidgetRepresentation3D_h

#include "vtkSlicerMarkupsModuleVTKWidgetsExport.h"
#include "vtkSlicerMarkupsWidgetRepresentation.h"

#include <map>

class vtkActor;
class vtkActor2D;
class vtkCamera;
class vtkCellPicker;
class vtkFastSelectVisiblePoints;
class vtkGlyph3DMapper;
class vtkLabelPlacementMapper;
class vtkPolyDataMapper;
class vtkProperty;

class vtkMRMLInteractionEventData;

class VTK_SLICER_MARKUPS_MODULE_VTKWIDGETS_EXPORT vtkSlicerMarkupsWidgetRepresentation3D : public vtkSlicerMarkupsWidgetRepresentation
{
public:
  /// Standard methods for instances of this class.
  vtkTypeMacro(vtkSlicerMarkupsWidgetRepresentation3D, vtkSlicerMarkupsWidgetRepresentation);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  void SetRenderer(vtkRenderer *ren) override;

  /// Subclasses of vtkSlicerMarkupsWidgetRepresentation3D must implement these methods. These
  /// are the methods that the widget and its representation use to
  /// communicate with each other.
  void UpdateFromMRML(vtkMRMLNode* caller, unsigned long event, void *callData = nullptr) override;

  /// Methods to make this class behave as a vtkProp.
  void GetActors(vtkPropCollection *) override;
  void ReleaseGraphicsResources(vtkWindow *) override;
  int RenderOverlay(vtkViewport *viewport) override;
  int RenderOpaqueGeometry(vtkViewport *viewport) override;
  int RenderTranslucentPolygonalGeometry(vtkViewport *viewport) override;
  vtkTypeBool HasTranslucentPolygonalGeometry() override;

  /// Return the bounds of the representation
  double *GetBounds() override;

  void CanInteract(vtkMRMLInteractionEventData* interactionEventData,
    int &foundComponentType, int &foundComponentIndex, double &closestDistance2) override;

  /// Check if interaction with the transformation handles is possible
  virtual void CanInteractWithHandles(vtkMRMLInteractionEventData* interactionEventData,
    int& foundComponentType, int& foundComponentIndex, double& closestDistance2);

  /// Checks if interaction with straight line between visible points is possible.
  /// Can be used on the output of CanInteract, as if no better component is found then the input is returned.
  void CanInteractWithLine(vtkMRMLInteractionEventData* interactionEventData,
    int &foundComponentType, int &foundComponentIndex, double &closestDistance2);

  bool AccuratePick(int x, int y, double pickPoint[3]);

  /// Return true if the control point is actually visible
  /// (displayed and not occluded by other objects in the view).
  /// Useful for non-regression tests that need to inspect internal state of the widget.
  bool GetNthControlPointViewVisibility(int n);

  /// Relative offset used for rendering occluded actors.
  /// The range of coincident offset can be between +/- 65000.
  /// Positive values move the occluded objects away from the camera, and negative values towards.
  /// Default value is -25000.
  vtkSetMacro(OccludedRelativeOffset, double);
  vtkGetMacro(OccludedRelativeOffset, double);

protected:
  vtkSlicerMarkupsWidgetRepresentation3D();
  ~vtkSlicerMarkupsWidgetRepresentation3D() override;

  double GetViewScaleFactorAtPosition(double positionWorld[3], vtkMRMLInteractionEventData* interactionEventData = nullptr);

  void UpdateViewScaleFactor() override;

  void UpdateControlPointSize() override;

  void UpdateInteractionPipeline() override;

  class ControlPointsPipeline3D : public ControlPointsPipeline
  {
  public:
    ControlPointsPipeline3D();
    ~ControlPointsPipeline3D() override;

    /// Orientation of the glyphs, represented as an array of quaternions
    vtkSmartPointer<vtkDoubleArray>   GlyphOrientationArray;

    vtkSmartPointer<vtkGlyph3DMapper> GlyphMapper;

    // Properties used to control the appearance of selected objects and
    // the manipulator in general.
    vtkSmartPointer<vtkProperty>     Property;
    vtkSmartPointer<vtkProperty>     OccludedProperty;
    vtkSmartPointer<vtkTextProperty> OccludedTextProperty;

    vtkSmartPointer<vtkPolyData> VisiblePointsPolyData;

    vtkSmartPointer<vtkFastSelectVisiblePoints>      SelectVisiblePoints;

    vtkSmartPointer<vtkIdTypeArray>              ControlPointIndices;  // store original ID to determine which control point is actually visible
    vtkSmartPointer<vtkPointSetToLabelHierarchy> OccludedPointSetToLabelHierarchyFilter;

    vtkSmartPointer<vtkGlyph3DMapper>        OccludedGlyphMapper;
    vtkSmartPointer<vtkLabelPlacementMapper> LabelsMapper;
    vtkSmartPointer<vtkLabelPlacementMapper> LabelsOccludedMapper;

    vtkSmartPointer<vtkActor>   Actor;
    vtkSmartPointer<vtkActor>   OccludedActor;
    vtkSmartPointer<vtkActor2D> LabelsActor;
    vtkSmartPointer<vtkActor2D> LabelsOccludedActor;
  };

  class ControlPointsPipelineManager
  {
  public:
    static constexpr int NumberOfControlPointTypes = vtkSlicerMarkupsWidgetRepresentation::NumberOfControlPointTypes;
    ControlPointsPipelineManager(vtkSlicerMarkupsWidgetRepresentation3D& parent);
    ControlPointsPipelineManager(const ControlPointsPipelineManager&) = delete;
    ControlPointsPipelineManager& operator=(const ControlPointsPipelineManager&) = delete;
    ControlPointsPipelineManager(ControlPointsPipelineManager&&) = delete;
    ControlPointsPipelineManager& operator=(ControlPointsPipelineManager&&) = delete;
    ~ControlPointsPipelineManager() = default;

    void GetActors(vtkPropCollection *pc);
    void ReleaseGraphicsResources(vtkWindow *win);
    int RenderOpaqueGeometry(vtkViewport *viewport, bool updateControlPointSize, double controlPointSize);
    int RenderTranslucentPolygonalGeometry(vtkViewport *viewport, vtkInformation* propertyKeys);
    vtkTypeBool HasTranslucentPolygonalGeometry();
    void UpdateAllPointsAndLabels(vtkMRMLMarkupsDisplayNode* markupsDisplayNode, double controlPointSize);
    //Gets actors for Unselected, Selected, and Active pipelines
    std::vector<vtkProp*> GetUSAActors();
    void PrintSelf(ostream& os, vtkIndent indent);
    void SetRenderer(vtkRenderer* ren);
    void SetToleranceWorld(const double tolerance);
    bool GetNthControlPointViewVisibility(int n, bool nIsSelected);
    vtkSmartPointer<vtkTextProperty> GetTextProperty(int controlPointType);
    vtkSmartPointer<vtkProperty> GetProperty(int controlPointType);
    vtkSmartPointer<vtkProperty> GetOccludedProperty(int controlPointType);
    //sets color of actor, text, and occluded
    void SetColor(int controlPointType, double a[3]);
    void SetColor(int controlPointType, double r, double g, double b);
    void SetOpacity(int controlPointType, double opacity, double textBackgroundOpacity);
    void SetOccludedOpacity(int controlPointType, double opacity, double textBackgroundOpacity);
    void SetFontSize(int controlPointType, int size);
    void SetGlyph3D();
    void SetGlyph2D(int type);
    void UpdateControlPointGlyphOrientation(vtkCamera* cam);
    void UpdateZBuffer(vtkFloatArray*& zBuffer);
    void SetAllPointsVisible();
    int RenderOverlay(vtkViewport* viewport);

  private:
    //Minimize use of this. Will take away later
    vtkSlicerMarkupsWidgetRepresentation3D& Parent;
    ControlPointsPipeline3D ControlPointsPipelines[NumberOfControlPointTypes]; // Unselected, Selected, Active, Project, ProjectBehind
  };

  virtual void UpdateControlPointGlyphOrientation();

  virtual void UpdateNthPointAndLabelFromMRML(int n);

  virtual void UpdateAllPointsAndLabelsFromMRML();

  /// Update the occluded relative offsets for an occluded mapper
  /// Allows occluded regions to be rendered on top.
  /// Sets the following parameter on the mappers:
  /// - RelativeCoincidentTopologyLineOffsetParameters
  /// - RelativeCoincidentTopologyPolygonOffsetParameters
  /// - RelativeCoincidentTopologyPointOffsetParameter
  void UpdateRelativeCoincidentTopologyOffsets(vtkMapper* mapper, vtkMapper* occludedMapper);
  using vtkMRMLAbstractWidgetRepresentation::UpdateRelativeCoincidentTopologyOffsets;

  vtkSmartPointer<vtkCellPicker> AccuratePicker;

  double TextActorPositionWorld[3];
  bool TextActorOccluded;
  bool HideTextActorIfAllPointsOccluded;
  double OccludedRelativeOffset;

  static std::map<vtkRenderer*, vtkSmartPointer<vtkFloatArray> > CachedZBuffers;

  vtkSmartPointer<vtkCallbackCommand> RenderCompletedCallback;
  static void OnRenderCompleted(vtkObject* caller, unsigned long event, void* clientData, void* callData);
  static vtkFloatArray* GetCachedZBuffer(vtkRenderer* renderer);

  //TODO: make private
  ControlPointsPipelineManager ControlPointsPipelines;
private:

  vtkSlicerMarkupsWidgetRepresentation3D(const vtkSlicerMarkupsWidgetRepresentation3D&) = delete;
  void operator=(const vtkSlicerMarkupsWidgetRepresentation3D&) = delete;
};

#endif
