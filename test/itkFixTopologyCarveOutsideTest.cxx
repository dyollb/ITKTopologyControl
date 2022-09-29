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

#include "itkFixTopologyCarveOutside.h"

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
itkFixTopologyCarveOutsideTest(int argc, char * argv[])
{
  if (argc < 2)
  {
    std::cerr << "Missing parameters." << std::endl;
    std::cerr << "Usage: " << itkNameOfTestExecutableMacro(argv);
    std::cerr << " outputImage [inputImage]";
    std::cerr << std::endl;
    return EXIT_FAILURE;
  }
  const char * outputImageFileName = (argc >= 2) ? argv[1] : "itkFixTopologyCarveOutsideTestOutput.mha";

  constexpr unsigned int Dimension = 3;
  using PixelType = int;
  using ImageType = itk::Image<PixelType, Dimension>;
  using RangeType = itk::ImageRegionRange<ImageType>;

  auto filter = itk::FixTopologyCarveOutside<ImageType, ImageType>::New();

  ITK_EXERCISE_BASIC_OBJECT_METHODS(filter, FixTopologyCarveOutside, ImageToImageFilter);

  ImageType::Pointer image;
  if (argc <= 2)
  {
    // Create input image to avoid test dependencies.
    image = ImageType::New();
    image->SetRegions({ 128, 128, 128 });
    image->Allocate();
    image->FillBuffer(0);

    ImageType::RegionType region;
    region.SetIndex({ 0, 0, 20 });
    region.SetSize({ 128, 128, 1 });
    for (auto & pixel : RangeType(*image, region))
    {
      pixel = 1;
    }

    region.SetIndex({ 45, 45, 20 });
    region.SetSize({ 5, 5, 1 });
    for (auto & pixel : RangeType(*image, region))
    {
      pixel = 0;
    }
  }
  else
  {
    const char * inputImageFileName = argv[2];
    auto reader = itk::ImageFileReader<ImageType>::New();
    reader->SetFileName(inputImageFileName);
    reader->Update();
    image = reader->GetOutput();
  }

  ShowProgress::Pointer showProgress = ShowProgress::New();
  filter->AddObserver(itk::ProgressEvent(), showProgress);
  filter->SetInput(image);
  filter->SetRadius(3);

  using WriterType = itk::ImageFileWriter<ImageType>;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(outputImageFileName);
  writer->SetInput(filter->GetOutput());
  writer->SetUseCompression(true);

  ITK_TRY_EXPECT_NO_EXCEPTION(writer->Update());

  std::cout << "Test finished." << std::endl;
  return EXIT_SUCCESS;
}
