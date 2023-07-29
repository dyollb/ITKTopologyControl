/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include "itkFixTopologyCarveInside.h"

#include "itkCommand.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkTestingMacros.h"

namespace
{
class ShowProgress : public itk::Command
{
public:
  itkNewMacro(ShowProgress);

  void
  Execute(itk::Object * caller, const itk::EventObject & event) override
  {
    Execute((const itk::Object *)caller, event);
  }

  void
  Execute(const itk::Object * caller, const itk::EventObject & event) override
  {
    if (!itk::ProgressEvent().CheckEvent(&event))
    {
      return;
    }
    const auto * processObject = dynamic_cast<const itk::ProcessObject *>(caller);
    if (!processObject)
    {
      return;
    }
    std::cout << " " << processObject->GetProgress();
  }
};
} // namespace

int
itkFixTopologyCarveInsideTest(int argc, char * argv[])
{
  if (argc < 2)
  {
    std::cerr << "Missing parameters." << std::endl;
    std::cerr << "Usage: " << itkNameOfTestExecutableMacro(argv);
    std::cerr << " outputImage [inputImage]";
    std::cerr << std::endl;
    return EXIT_FAILURE;
  }
  const char * outputImageFileName = (argc >= 2) ? argv[1] : "itkFixTopologyCarveInsideTestOutput.mha";

  constexpr unsigned int Dimension = 3;
  using PixelType = int;
  using ImageType = itk::Image<PixelType, Dimension>;
  using RangeType = itk::ImageRegionRange<ImageType>;
  using FilterType = itk::FixTopologyCarveInside<ImageType, ImageType>;

  auto filter = FilterType::New();

  ITK_EXERCISE_BASIC_OBJECT_METHODS(filter, FixTopologyCarveInside, FixTopologyBase);

  ImageType::Pointer image;
  if (argc <= 2)
  {
    // Create input image to avoid test dependencies.
    image = ImageType::New();
    image->SetRegions({ 128, 128, 128 });
    image->Allocate();
    image->FillBuffer(0);

    ImageType::RegionType region;
    region.SetIndex({ 20, 20, 20 });
    region.SetSize({ 20, 20, 20 });
    for (auto & pixel : RangeType(*image, region))
    {
      pixel = 1;
    }

    region.SetIndex({ 42, 20, 20 });
    region.SetSize({ 20, 20, 20 });
    for (auto & pixel : RangeType(*image, region))
    {
      pixel = 1;
    }

    region.SetIndex({ 40, 34, 34 });
    region.SetSize({ 2, 5, 5 });
    for (auto & pixel : RangeType(*image, region))
    {
      pixel = 1;
    }

    image->SetPixel({ 40, 24, 24 }, 1);
    image->SetPixel({ 41, 24, 24 }, 1);

    using IndexType = ImageType::IndexType;
    using NeighborhoodIteratorType = itk::NeighborhoodIterator<ImageType, itk::ConstantBoundaryCondition<ImageType>>;
    NeighborhoodIteratorType n_it({ 1, 1, 1 }, image, region);

    const auto get_mask = [&n_it](const IndexType & idx, const int FG) {
      n_it.SetLocation(idx);
      auto n = n_it.GetNeighborhood();
      for (auto & v : n.GetBufferReference())
        v = (v == 1) ? FG : 1 - FG;
      return n;
    };
    auto vals = get_mask({ 40, 24, 24 }, 1);

    std::cerr << "topology::EulerInvariant(vals, 0): " << topology::EulerInvariant(vals, 0) << "\n";
    std::cerr << "topology::EulerInvariant(vals, 1): " << topology::EulerInvariant(vals, 1) << "\n";
    std::cerr << "topology::CCInvariant(vals, 0): " << topology::CCInvariant(vals, 0) << "\n";
    std::cerr << "topology::CCInvariant(vals, 1): " << topology::CCInvariant(vals, 1) << "\n";
    std::cerr << "topology::NonmanifoldRemove(vals, 0): " << topology::NonmanifoldRemove(vals, 0) << "\n";
    std::cerr << "topology::NonmanifoldRemove(vals, 1): " << topology::NonmanifoldRemove(vals, 1) << "\n";

  }
  else
  {
    const char * inputImageFileName = argv[2];
    auto         reader = itk::ImageFileReader<ImageType>::New();
    reader->SetFileName(inputImageFileName);
    reader->Update();
    image = reader->GetOutput();
  }


  auto mask = FilterType::MaskImageType::New();
  mask->SetRegions(image->GetBufferedRegion().GetSize());
  mask->Allocate();
  mask->FillBuffer(0);
  mask->SetPixel({ 30, 30, 30 }, 1);
  mask->SetPixel({ 51, 30, 30 }, 1);

  auto showProgress = ShowProgress::New();
  // filter->AddObserver(itk::ProgressEvent(), showProgress);
  filter->SetInput(image);
  filter->SetMaskImage(mask);

  auto writer = itk::ImageFileWriter<ImageType>::New();
  writer->SetFileName(outputImageFileName);
  writer->SetInput(filter->GetOutput());
  writer->SetUseCompression(true);

  ITK_TRY_EXPECT_NO_EXCEPTION(writer->Update());

  std::cout << "Test finished." << std::endl;
  return EXIT_SUCCESS;
}