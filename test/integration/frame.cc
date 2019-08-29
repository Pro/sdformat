/*
 * Copyright 2015 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include "sdf/Element.hh"
#include "sdf/Frame.hh"
#include "sdf/Filesystem.hh"
#include "sdf/Link.hh"
#include "sdf/Model.hh"
#include "sdf/Root.hh"
#include "sdf/SDFImpl.hh"
#include "sdf/World.hh"
#include "sdf/parser.hh"
#include "sdf/sdf_config.h"

#include "test_config.h"

////////////////////////////////////////
// Test parsing a model element that has a frame element
TEST(Frame, ModelFrame)
{
  std::ostringstream stream;
  std::string version = SDF_VERSION;
  stream
    << "<sdf version='" << version << "'>"
    << "<model name='my_model'>"
    << "  <frame name='mframe'>"
    << "    <pose relative_to='/world'>1 1 0 0 0 0</pose>"
    << "  </frame>"
    << "  <pose relative_to='mframe'>1 0 0 0 0 0</pose>"
    << "  <link name='link'/>"
    << "</model>"
    << "</sdf>";

  sdf::SDFPtr sdfParsed(new sdf::SDF());
  sdf::init(sdfParsed);
  ASSERT_TRUE(sdf::readString(stream.str(), sdfParsed));

  // Verify correct parsing

  // model
  EXPECT_TRUE(sdfParsed->Root()->HasElement("model"));
  sdf::ElementPtr modelElem = sdfParsed->Root()->GetElement("model");
  EXPECT_TRUE(modelElem->HasAttribute("name"));
  EXPECT_EQ(modelElem->Get<std::string>("name"), "my_model");

  // model frame
  EXPECT_TRUE(modelElem->HasElement("frame"));
  sdf::ElementPtr frameElem = modelElem->GetElement("frame");
  EXPECT_TRUE(frameElem->HasAttribute("name"));
  EXPECT_EQ(frameElem->Get<std::string>("name"), "mframe");

  // model frame pose
  EXPECT_TRUE(frameElem->HasElement("pose"));
  sdf::ElementPtr poseElem = frameElem->GetElement("pose");
  EXPECT_TRUE(poseElem->HasAttribute("relative_to"));
  EXPECT_EQ(poseElem->Get<std::string>("relative_to"), "/world");
  EXPECT_EQ(poseElem->Get<ignition::math::Pose3d>(),
            ignition::math::Pose3d(1, 1, 0, 0, 0, 0));

  // model pose
  EXPECT_TRUE(modelElem->HasElement("pose"));
  sdf::ElementPtr modelPoseElem = modelElem->GetElement("pose");
  EXPECT_TRUE(modelPoseElem->HasAttribute("relative_to"));
  EXPECT_EQ(modelPoseElem->Get<std::string>("relative_to"), "mframe");
  EXPECT_EQ(modelPoseElem->Get<ignition::math::Pose3d>(),
            ignition::math::Pose3d(1, 0, 0, 0, 0, 0));

  // link
  EXPECT_TRUE(modelElem->HasElement("link"));
  sdf::ElementPtr linkElem = modelElem->GetElement("link");
  EXPECT_TRUE(linkElem->HasAttribute("name"));
  EXPECT_EQ(linkElem->Get<std::string>("name"), "link");
}

////////////////////////////////////////
// Test parsing a model element with an empty frame element
TEST(Frame, FrameDefaultPose)
{
  std::ostringstream stream;
  std::string version = SDF_VERSION;
  stream
    << "<sdf version='" << version << "'>"
    << "<model name='my_model'>"
    << "  <frame name='mframe'/>"
    << "  <link name='link'/>"
    << "</model>"
    << "</sdf>";

  sdf::SDFPtr sdfParsed(new sdf::SDF());
  sdf::init(sdfParsed);
  ASSERT_TRUE(sdf::readString(stream.str(), sdfParsed));

  // Verify correct parsing

  // model
  EXPECT_TRUE(sdfParsed->Root()->HasElement("model"));
  sdf::ElementPtr modelElem = sdfParsed->Root()->GetElement("model");
  EXPECT_TRUE(modelElem->HasAttribute("name"));
  EXPECT_EQ(modelElem->Get<std::string>("name"), "my_model");

  // model frame
  EXPECT_TRUE(modelElem->HasElement("frame"));
  sdf::ElementPtr frameElem = modelElem->GetElement("frame");
  EXPECT_TRUE(frameElem->HasAttribute("name"));
  EXPECT_EQ(frameElem->Get<std::string>("name"), "mframe");

  // model frame pose
  EXPECT_TRUE(!frameElem->HasElement("pose"));
  sdf::ElementPtr poseElem = frameElem->GetElement("pose");
  EXPECT_TRUE(poseElem->HasAttribute("relative_to"));
  EXPECT_EQ(poseElem->Get<std::string>("relative_to"), "");
  EXPECT_EQ(poseElem->Get<ignition::math::Pose3d>(),
            ignition::math::Pose3d(0, 0, 0, 0, 0, 0));

  // link
  EXPECT_TRUE(modelElem->HasElement("link"));
  sdf::ElementPtr linkElem = modelElem->GetElement("link");
  EXPECT_TRUE(linkElem->HasAttribute("name"));
  EXPECT_EQ(linkElem->Get<std::string>("name"), "link");
}

////////////////////////////////////////
// Test parsing a model element with no frames - for backward compatibility
TEST(Frame, NoFrame)
{
  std::ostringstream stream;
  std::string version = SDF_VERSION;
  stream
    << "<sdf version='" << version << "'>"
    << "<model name='my_model'>"
    << "  <link name='link'/>"
    << "</model>"
    << "</sdf>";

  sdf::SDFPtr sdfParsed(new sdf::SDF());
  sdf::init(sdfParsed);
  ASSERT_TRUE(sdf::readString(stream.str(), sdfParsed));

  // Verify correct parsing

  // model
  EXPECT_TRUE(sdfParsed->Root()->HasElement("model"));
  sdf::ElementPtr modelElem = sdfParsed->Root()->GetElement("model");
  EXPECT_TRUE(modelElem->HasAttribute("name"));
  EXPECT_EQ(modelElem->Get<std::string>("name"), "my_model");

  {
    // model frame
    EXPECT_TRUE(!modelElem->HasElement("frame"));
    sdf::ElementPtr frameElem = modelElem->GetElement("frame");
    EXPECT_TRUE(frameElem->HasAttribute("name"));
    EXPECT_EQ(frameElem->Get<std::string>("name"), "");

    // model frame pose
    EXPECT_TRUE(!frameElem->HasElement("pose"));
    sdf::ElementPtr poseElem = frameElem->GetElement("pose");
    EXPECT_TRUE(poseElem->HasAttribute("relative_to"));
    EXPECT_EQ(poseElem->Get<std::string>("relative_to"), "");
    EXPECT_EQ(poseElem->Get<ignition::math::Pose3d>(),
              ignition::math::Pose3d(0, 0, 0, 0, 0, 0));
  }

  // link
  EXPECT_TRUE(modelElem->HasElement("link"));
  sdf::ElementPtr linkElem = modelElem->GetElement("link");
  EXPECT_TRUE(linkElem->HasAttribute("name"));
  EXPECT_EQ(linkElem->Get<std::string>("name"), "link");

  {
    // link pose
    EXPECT_TRUE(!linkElem->HasElement("pose"));
    sdf::ElementPtr poseElem = linkElem->GetElement("pose");
    EXPECT_TRUE(poseElem->HasAttribute("relative_to"));
    EXPECT_EQ(poseElem->Get<std::string>("relative_to"), "");
    EXPECT_EQ(poseElem->Get<ignition::math::Pose3d>(),
              ignition::math::Pose3d(0, 0, 0, 0, 0, 0));
  }
}

////////////////////////////////////////
// Test parsing nested model states
TEST(Frame, StateFrame)
{
  std::ostringstream sdfStr;
  sdfStr << "<sdf version ='" << SDF_VERSION << "'>"
    << "<world name='default'>"
    << "<state world_name='default'>"
    << "<model name='my_model'>"
    << "  <frame name='mframe'>"
    << "    <pose relative_to='/world'>1 0 2 0 0 0</pose>"
    << "  </frame>"
    << "  <pose relative_to='mframe'>3 3 9 0 0 0</pose>"
    << "  <link name='my_link'>"
    << "    <pose relative_to='lframe'>111 3 0 0 0 0</pose>"
    << "  </link>"
    << "</model>"
    << "<light name='my_light'>"
    << "    <pose relative_to='lframe'>99 0 22 0 0 0</pose>"
    << "</light>"
    << "</state>"
    << "</world>"
    << "</sdf>";

  sdf::SDFPtr sdfParsed(new sdf::SDF());
  sdf::init(sdfParsed);
  ASSERT_TRUE(sdf::readString(sdfStr.str(), sdfParsed));

  // load the state sdf
  EXPECT_TRUE(sdfParsed->Root()->HasElement("world"));
  sdf::ElementPtr worldElem = sdfParsed->Root()->GetElement("world");
  EXPECT_TRUE(worldElem->HasElement("state"));
  sdf::ElementPtr stateElem = worldElem->GetElement("state");

  EXPECT_TRUE(stateElem->HasElement("model"));
  sdf::ElementPtr modelStateElem = stateElem->GetElement("model");

  // model
  EXPECT_TRUE(modelStateElem->HasAttribute("name"));
  EXPECT_EQ(modelStateElem->Get<std::string>("name"), "my_model");

  {
    // model frame
    EXPECT_TRUE(modelStateElem->HasElement("frame"));
    sdf::ElementPtr frameElem = modelStateElem->GetElement("frame");
    EXPECT_TRUE(frameElem->HasAttribute("name"));
    EXPECT_EQ(frameElem->Get<std::string>("name"), "mframe");

    // model frame pose
    EXPECT_TRUE(frameElem->HasElement("pose"));
    sdf::ElementPtr poseElem = frameElem->GetElement("pose");
    EXPECT_TRUE(poseElem->HasAttribute("relative_to"));
    EXPECT_EQ(poseElem->Get<std::string>("relative_to"), "/world");
    EXPECT_EQ(poseElem->Get<ignition::math::Pose3d>(),
              ignition::math::Pose3d(1, 0, 2, 0, 0, 0));

    // model pose
    EXPECT_TRUE(modelStateElem->HasElement("pose"));
    sdf::ElementPtr modelPoseElem = modelStateElem->GetElement("pose");
    EXPECT_TRUE(modelPoseElem->HasAttribute("relative_to"));
    EXPECT_EQ(modelPoseElem->Get<std::string>("relative_to"), "mframe");
    EXPECT_EQ(modelPoseElem->Get<ignition::math::Pose3d>(),
              ignition::math::Pose3d(3, 3, 9, 0, 0, 0));
  }

  // link
  EXPECT_TRUE(modelStateElem->HasElement("link"));
  sdf::ElementPtr linkStateElem = modelStateElem->GetElement("link");
  EXPECT_TRUE(linkStateElem->HasAttribute("name"));
  EXPECT_EQ(linkStateElem->Get<std::string>("name"), "my_link");

  {
    // link pose
    EXPECT_TRUE(linkStateElem->HasElement("pose"));
    sdf::ElementPtr linkPoseElem = linkStateElem->GetElement("pose");
    EXPECT_TRUE(linkPoseElem->HasAttribute("relative_to"));
    EXPECT_EQ(linkPoseElem->Get<std::string>("relative_to"), "lframe");
    EXPECT_EQ(linkPoseElem->Get<ignition::math::Pose3d>(),
              ignition::math::Pose3d(111, 3, 0, 0, 0, 0));
  }

  EXPECT_TRUE(stateElem->HasElement("light"));
  sdf::ElementPtr lightStateElem = stateElem->GetElement("light");

  // light
  EXPECT_TRUE(lightStateElem->HasAttribute("name"));
  EXPECT_EQ(lightStateElem->Get<std::string>("name"), "my_light");

  {
    // light pose
    EXPECT_TRUE(lightStateElem->HasElement("pose"));
    sdf::ElementPtr lightPoseElem = lightStateElem->GetElement("pose");
    EXPECT_TRUE(lightPoseElem->HasAttribute("relative_to"));
    EXPECT_EQ(lightPoseElem->Get<std::string>("relative_to"), "lframe");
    EXPECT_EQ(lightPoseElem->Get<ignition::math::Pose3d>(),
              ignition::math::Pose3d(99, 0, 22, 0, 0, 0));
  }
}

////////////////////////////////////////
// Test parsing a include element that has a pose element
TEST(Frame, IncludeRelativeTo)
{
  const std::string MODEL_PATH =
    sdf::filesystem::append(PROJECT_SOURCE_PATH, "test", "integration",
                            "model", "box");

  std::ostringstream stream;
  std::string version = SDF_VERSION;
  stream
    << "<sdf version='" << version << "'>"
    << "<world name='default'>"
    << "<include>"
    << "  <name>my_model</name>"
    << "  <pose relative_to='/world'>5 -2 1 0 0 0</pose>"
    << "  <uri>" + MODEL_PATH +"</uri>"
    << "</include>"
    << "</world>"
    << "</sdf>";


  sdf::SDFPtr sdfParsed(new sdf::SDF());
  sdf::init(sdfParsed);
  ASSERT_TRUE(sdf::readString(stream.str(), sdfParsed));

  // Verify correct parsing

  // model
  EXPECT_TRUE(sdfParsed->Root()->HasElement("world"));
  sdf::ElementPtr worldElem = sdfParsed->Root()->GetElement("world");

  EXPECT_TRUE(worldElem->HasElement("model"));
  sdf::ElementPtr modelElem = worldElem->GetElement("model");
  EXPECT_TRUE(modelElem->HasAttribute("name"));
  EXPECT_EQ(modelElem->Get<std::string>("name"), "my_model");

  // model pose
  EXPECT_TRUE(modelElem->HasElement("pose"));
  sdf::ElementPtr modelPoseElem = modelElem->GetElement("pose");
  EXPECT_TRUE(modelPoseElem->HasAttribute("relative_to"));
  EXPECT_EQ(modelPoseElem->Get<std::string>("relative_to"), "/world");
  EXPECT_EQ(modelPoseElem->Get<ignition::math::Pose3d>(),
            ignition::math::Pose3d(5, -2, 1, 0, 0, 0));
}

/////////////////////////////////////////////////
TEST(DOMFrame, LoadModelFramesAttachedTo)
{
  const std::string testFile =
    sdf::filesystem::append(PROJECT_SOURCE_PATH, "test", "sdf",
        "model_frame_attached_to.sdf");

  // Load the SDF file
  sdf::Root root;
  EXPECT_TRUE(root.Load(testFile).empty());

  // Get the first model
  const sdf::Model *model = root.ModelByIndex(0);
  ASSERT_NE(nullptr, model);
  EXPECT_EQ("model_frame_attached_to", model->Name());
  EXPECT_EQ(1u, model->LinkCount());
  EXPECT_NE(nullptr, model->LinkByIndex(0));
  EXPECT_EQ(nullptr, model->LinkByIndex(1));
  EXPECT_EQ(ignition::math::Pose3d(0, 0, 0, 0, 0, 0), model->Pose());
  EXPECT_EQ("", model->PoseRelativeTo());

  EXPECT_TRUE(model->LinkNameExists("L"));

  EXPECT_TRUE(model->CanonicalLinkName().empty());

  EXPECT_EQ(0u, model->JointCount());
  EXPECT_EQ(nullptr, model->JointByIndex(0));

  EXPECT_EQ(4u, model->FrameCount());
  EXPECT_NE(nullptr, model->FrameByIndex(0));
  EXPECT_NE(nullptr, model->FrameByIndex(1));
  EXPECT_NE(nullptr, model->FrameByIndex(2));
  EXPECT_NE(nullptr, model->FrameByIndex(3));
  EXPECT_EQ(nullptr, model->FrameByIndex(4));
  ASSERT_TRUE(model->FrameNameExists("F00"));
  ASSERT_TRUE(model->FrameNameExists("F0"));
  ASSERT_TRUE(model->FrameNameExists("F1"));
  ASSERT_TRUE(model->FrameNameExists("F2"));

  EXPECT_TRUE(model->FrameByName("F00")->AttachedTo().empty());
  EXPECT_TRUE(model->FrameByName("F0")->AttachedTo().empty());
  EXPECT_EQ("L", model->FrameByName("F1")->AttachedTo());
  EXPECT_EQ("F1", model->FrameByName("F2")->AttachedTo());

  EXPECT_TRUE(model->FrameByName("F00")->PoseRelativeTo().empty());
  EXPECT_TRUE(model->FrameByName("F0")->PoseRelativeTo().empty());
  EXPECT_TRUE(model->FrameByName("F1")->PoseRelativeTo().empty());
  EXPECT_TRUE(model->FrameByName("F2")->PoseRelativeTo().empty());
}

/////////////////////////////////////////////////
TEST(DOMFrame, LoadModelFramesInvalidAttachedTo)
{
  const std::string testFile =
    sdf::filesystem::append(PROJECT_SOURCE_PATH, "test", "sdf",
        "model_frame_invalid_attached_to.sdf");

  // Load the SDF file
  sdf::Root root;
  EXPECT_TRUE(root.Load(testFile).empty());

  // Get the first model
  const sdf::Model *model = root.ModelByIndex(0);
  ASSERT_NE(nullptr, model);
  EXPECT_EQ("model_frame_invalid_attached_to", model->Name());
  EXPECT_EQ(1u, model->LinkCount());
  EXPECT_NE(nullptr, model->LinkByIndex(0));
  EXPECT_EQ(nullptr, model->LinkByIndex(1));
  EXPECT_EQ(ignition::math::Pose3d(0, 0, 0, 0, 0, 0), model->Pose());
  EXPECT_EQ("", model->PoseRelativeTo());

  EXPECT_TRUE(model->LinkNameExists("L"));

  EXPECT_TRUE(model->CanonicalLinkName().empty());

  EXPECT_EQ(0u, model->JointCount());
  EXPECT_EQ(nullptr, model->JointByIndex(0));

  EXPECT_EQ(4u, model->FrameCount());
  EXPECT_NE(nullptr, model->FrameByIndex(0));
  EXPECT_NE(nullptr, model->FrameByIndex(1));
  EXPECT_NE(nullptr, model->FrameByIndex(2));
  EXPECT_NE(nullptr, model->FrameByIndex(3));
  EXPECT_EQ(nullptr, model->FrameByIndex(4));
  ASSERT_TRUE(model->FrameNameExists("F1"));
  ASSERT_TRUE(model->FrameNameExists("F2"));
  ASSERT_TRUE(model->FrameNameExists("F3"));
  ASSERT_TRUE(model->FrameNameExists("F4"));

  EXPECT_EQ("L", model->FrameByName("F1")->AttachedTo());
  EXPECT_EQ("F1", model->FrameByName("F2")->AttachedTo());
  EXPECT_EQ("A", model->FrameByName("F3")->AttachedTo());
  EXPECT_EQ("F4", model->FrameByName("F4")->AttachedTo());

  EXPECT_TRUE(model->FrameByName("F1")->PoseRelativeTo().empty());
  EXPECT_TRUE(model->FrameByName("F2")->PoseRelativeTo().empty());
  EXPECT_TRUE(model->FrameByName("F3")->PoseRelativeTo().empty());
  EXPECT_TRUE(model->FrameByName("F4")->PoseRelativeTo().empty());
}

/////////////////////////////////////////////////
TEST(DOMFrame, LoadModelFramesAttachedToJoint)
{
  const std::string testFile =
    sdf::filesystem::append(PROJECT_SOURCE_PATH, "test", "sdf",
        "model_frame_attached_to_joint.sdf");

  // Load the SDF file
  sdf::Root root;
  EXPECT_TRUE(root.Load(testFile).empty());

  // Get the first model
  const sdf::Model *model = root.ModelByIndex(0);
  ASSERT_NE(nullptr, model);
  EXPECT_EQ("model_frame_attached_to_joint", model->Name());
  EXPECT_EQ(2u, model->LinkCount());
  EXPECT_NE(nullptr, model->LinkByIndex(0));
  EXPECT_NE(nullptr, model->LinkByIndex(1));
  EXPECT_EQ(nullptr, model->LinkByIndex(2));
  EXPECT_EQ(ignition::math::Pose3d(0, 0, 0, 0, 0, 0), model->Pose());
  EXPECT_EQ("", model->PoseRelativeTo());

  EXPECT_TRUE(model->LinkNameExists("P"));
  EXPECT_TRUE(model->LinkNameExists("C"));

  EXPECT_TRUE(model->CanonicalLinkName().empty());

  EXPECT_EQ(1u, model->JointCount());
  EXPECT_NE(nullptr, model->JointByIndex(0));
  EXPECT_EQ(nullptr, model->JointByIndex(1));

  EXPECT_TRUE(model->JointNameExists("J"));

  EXPECT_EQ(4u, model->FrameCount());
  EXPECT_NE(nullptr, model->FrameByIndex(0));
  EXPECT_NE(nullptr, model->FrameByIndex(1));
  EXPECT_NE(nullptr, model->FrameByIndex(2));
  EXPECT_NE(nullptr, model->FrameByIndex(3));
  EXPECT_EQ(nullptr, model->FrameByIndex(4));
  ASSERT_TRUE(model->FrameNameExists("F1"));
  ASSERT_TRUE(model->FrameNameExists("F2"));
  ASSERT_TRUE(model->FrameNameExists("F3"));
  ASSERT_TRUE(model->FrameNameExists("F4"));

  EXPECT_EQ("P", model->FrameByName("F1")->AttachedTo());
  EXPECT_EQ("C", model->FrameByName("F2")->AttachedTo());
  EXPECT_EQ("J", model->FrameByName("F3")->AttachedTo());
  EXPECT_EQ("F3", model->FrameByName("F4")->AttachedTo());

  EXPECT_TRUE(model->FrameByName("F1")->PoseRelativeTo().empty());
  EXPECT_TRUE(model->FrameByName("F2")->PoseRelativeTo().empty());
  EXPECT_TRUE(model->FrameByName("F3")->PoseRelativeTo().empty());
  EXPECT_TRUE(model->FrameByName("F4")->PoseRelativeTo().empty());
}

/////////////////////////////////////////////////
TEST(DOMFrame, LoadWorldFramesAttachedTo)
{
  const std::string testFile =
    sdf::filesystem::append(PROJECT_SOURCE_PATH, "test", "sdf",
        "world_frame_attached_to.sdf");

  // Load the SDF file
  sdf::Root root;
  EXPECT_TRUE(root.Load(testFile).empty());

  // Get the first world
  const sdf::World *world = root.WorldByIndex(0);
  ASSERT_NE(nullptr, world);
  EXPECT_EQ("world_frame_attached_to", world->Name());
  EXPECT_EQ(1u, world->ModelCount());
  EXPECT_NE(nullptr, world->ModelByIndex(0));
  EXPECT_EQ(nullptr, world->ModelByIndex(1));

  EXPECT_TRUE(world->ModelNameExists("M1"));

  const sdf::Model *model = world->ModelByIndex(0);
  ASSERT_NE(nullptr, model);
  EXPECT_EQ("M1", model->Name());
  EXPECT_EQ(1u, model->LinkCount());
  EXPECT_NE(nullptr, model->LinkByIndex(0));
  EXPECT_EQ(nullptr, model->LinkByIndex(1));
  EXPECT_EQ(1u, model->FrameCount());
  EXPECT_NE(nullptr, model->FrameByIndex(0));
  EXPECT_EQ(nullptr, model->FrameByIndex(1));
  ASSERT_TRUE(model->LinkNameExists("L"));
  ASSERT_TRUE(model->FrameNameExists("F0"));
  EXPECT_EQ("L", model->FrameByName("F0")->AttachedTo());

  EXPECT_EQ(4u, world->FrameCount());
  EXPECT_NE(nullptr, world->FrameByIndex(0));
  EXPECT_NE(nullptr, world->FrameByIndex(1));
  EXPECT_NE(nullptr, world->FrameByIndex(2));
  EXPECT_NE(nullptr, world->FrameByIndex(3));
  EXPECT_EQ(nullptr, world->FrameByIndex(4));
  ASSERT_TRUE(world->FrameNameExists("world"));
  ASSERT_TRUE(world->FrameNameExists("F0"));
  ASSERT_TRUE(world->FrameNameExists("F1"));
  ASSERT_TRUE(world->FrameNameExists("F2"));

  EXPECT_TRUE(world->FrameByName("world")->AttachedTo().empty());
  EXPECT_TRUE(world->FrameByName("F0")->AttachedTo().empty());
  EXPECT_EQ("F0", world->FrameByName("F1")->AttachedTo());
  EXPECT_EQ("M1", world->FrameByName("F2")->AttachedTo());

  EXPECT_TRUE(world->FrameByName("world")->PoseRelativeTo().empty());
  EXPECT_TRUE(world->FrameByName("F0")->PoseRelativeTo().empty());
  EXPECT_TRUE(world->FrameByName("F1")->PoseRelativeTo().empty());
  EXPECT_TRUE(world->FrameByName("F2")->PoseRelativeTo().empty());
}

/////////////////////////////////////////////////
TEST(DOMFrame, LoadWorldFramesInvalidAttachedTo)
{
  const std::string testFile =
    sdf::filesystem::append(PROJECT_SOURCE_PATH, "test", "sdf",
        "world_frame_invalid_attached_to.sdf");

  // Load the SDF file
  sdf::Root root;
  EXPECT_TRUE(root.Load(testFile).empty());

  // Get the first world
  const sdf::World *world = root.WorldByIndex(0);
  ASSERT_NE(nullptr, world);
  EXPECT_EQ("world_frame_invalid_attached_to", world->Name());
  EXPECT_EQ(0u, world->ModelCount());
  EXPECT_EQ(nullptr, world->ModelByIndex(0));

  EXPECT_EQ(2u, world->FrameCount());
  EXPECT_NE(nullptr, world->FrameByIndex(0));
  EXPECT_NE(nullptr, world->FrameByIndex(1));
  EXPECT_EQ(nullptr, world->FrameByIndex(2));
  ASSERT_TRUE(world->FrameNameExists("self_cycle"));
  ASSERT_TRUE(world->FrameNameExists("F"));

  EXPECT_EQ("A", world->FrameByName("F")->AttachedTo());
  EXPECT_EQ("self_cycle", world->FrameByName("self_cycle")->AttachedTo());
}

/////////////////////////////////////////////////
TEST(DOMFrame, LoadModelFramesRelativeTo)
{
  const std::string testFile =
    sdf::filesystem::append(PROJECT_SOURCE_PATH, "test", "sdf",
        "model_frame_relative_to.sdf");

  // Load the SDF file
  sdf::Root root;
  EXPECT_TRUE(root.Load(testFile).empty());

  // Get the first model
  const sdf::Model *model = root.ModelByIndex(0);
  ASSERT_NE(nullptr, model);
  EXPECT_EQ("model_frame_relative_to", model->Name());
  EXPECT_EQ(1u, model->LinkCount());
  EXPECT_NE(nullptr, model->LinkByIndex(0));
  EXPECT_EQ(nullptr, model->LinkByIndex(1));
  EXPECT_EQ(ignition::math::Pose3d(0, 0, 0, 0, 0, 0), model->Pose());
  EXPECT_EQ("", model->PoseRelativeTo());

  EXPECT_TRUE(model->LinkNameExists("L"));

  EXPECT_TRUE(model->CanonicalLinkName().empty());

  EXPECT_EQ(0u, model->JointCount());
  EXPECT_EQ(nullptr, model->JointByIndex(0));

  EXPECT_EQ(4u, model->FrameCount());
  EXPECT_NE(nullptr, model->FrameByIndex(0));
  EXPECT_NE(nullptr, model->FrameByIndex(1));
  EXPECT_NE(nullptr, model->FrameByIndex(2));
  EXPECT_NE(nullptr, model->FrameByIndex(3));
  EXPECT_EQ(nullptr, model->FrameByIndex(4));
  ASSERT_TRUE(model->FrameNameExists("F0"));
  ASSERT_TRUE(model->FrameNameExists("F1"));
  ASSERT_TRUE(model->FrameNameExists("F2"));
  ASSERT_TRUE(model->FrameNameExists("F3"));

  EXPECT_TRUE(model->FrameByName("F0")->AttachedTo().empty());
  EXPECT_EQ("L", model->FrameByName("F1")->AttachedTo());
  EXPECT_EQ("L", model->FrameByName("F2")->AttachedTo());
  EXPECT_TRUE(model->FrameByName("F3")->AttachedTo().empty());

  EXPECT_TRUE(model->FrameByName("F0")->PoseRelativeTo().empty());
  EXPECT_TRUE(model->FrameByName("F1")->PoseRelativeTo().empty());
  EXPECT_TRUE(model->FrameByName("F2")->PoseRelativeTo().empty());
  EXPECT_EQ("L", model->FrameByName("F3")->PoseRelativeTo());
}

/////////////////////////////////////////////////
TEST(DOMFrame, LoadModelFramesInvalidRelativeTo)
{
  const std::string testFile =
    sdf::filesystem::append(PROJECT_SOURCE_PATH, "test", "sdf",
        "model_invalid_frame_relative_to.sdf");

  // Load the SDF file
  sdf::Root root;
  EXPECT_TRUE(root.Load(testFile).empty());

  // Get the first model
  const sdf::Model *model = root.ModelByIndex(0);
  ASSERT_NE(nullptr, model);
  EXPECT_EQ("model_invalid_frame_relative_to", model->Name());
  EXPECT_EQ(1u, model->LinkCount());
  EXPECT_NE(nullptr, model->LinkByIndex(0));
  EXPECT_EQ(nullptr, model->LinkByIndex(1));
  EXPECT_EQ(ignition::math::Pose3d(0, 0, 0, 0, 0, 0), model->Pose());
  EXPECT_EQ("", model->PoseRelativeTo());

  EXPECT_TRUE(model->LinkNameExists("L"));

  EXPECT_TRUE(model->CanonicalLinkName().empty());

  EXPECT_EQ(0u, model->JointCount());
  EXPECT_EQ(nullptr, model->JointByIndex(0));

  EXPECT_EQ(2u, model->FrameCount());
  EXPECT_NE(nullptr, model->FrameByIndex(0));
  EXPECT_NE(nullptr, model->FrameByIndex(1));
  EXPECT_EQ(nullptr, model->FrameByIndex(2));
  ASSERT_TRUE(model->FrameNameExists("F"));
  ASSERT_TRUE(model->FrameNameExists("cycle"));

  EXPECT_TRUE(model->FrameByName("F")->AttachedTo().empty());
  EXPECT_TRUE(model->FrameByName("cycle")->AttachedTo().empty());

  EXPECT_EQ("A", model->FrameByName("F")->PoseRelativeTo());
  EXPECT_EQ("cycle", model->FrameByName("cycle")->PoseRelativeTo());
}

/////////////////////////////////////////////////
TEST(DOMFrame, LoadModelFramesRelativeToJoint)
{
  const std::string testFile =
    sdf::filesystem::append(PROJECT_SOURCE_PATH, "test", "sdf",
        "model_frame_relative_to_joint.sdf");

  // Load the SDF file
  sdf::Root root;
  EXPECT_TRUE(root.Load(testFile).empty());

  // Get the first model
  const sdf::Model *model = root.ModelByIndex(0);
  ASSERT_NE(nullptr, model);
  EXPECT_EQ("model_frame_relative_to_joint", model->Name());
  EXPECT_EQ(2u, model->LinkCount());
  EXPECT_NE(nullptr, model->LinkByIndex(0));
  EXPECT_NE(nullptr, model->LinkByIndex(1));
  EXPECT_EQ(nullptr, model->LinkByIndex(2));
  EXPECT_EQ(ignition::math::Pose3d(0, 0, 0, 0, 0, 0), model->Pose());
  EXPECT_EQ("", model->PoseRelativeTo());

  EXPECT_TRUE(model->LinkNameExists("P"));
  EXPECT_TRUE(model->LinkNameExists("C"));

  EXPECT_TRUE(model->CanonicalLinkName().empty());

  EXPECT_EQ(1u, model->JointCount());
  EXPECT_NE(nullptr, model->JointByIndex(0));
  EXPECT_EQ(nullptr, model->JointByIndex(1));

  EXPECT_TRUE(model->JointNameExists("J"));

  EXPECT_EQ(4u, model->FrameCount());
  EXPECT_NE(nullptr, model->FrameByIndex(0));
  EXPECT_NE(nullptr, model->FrameByIndex(1));
  EXPECT_NE(nullptr, model->FrameByIndex(2));
  EXPECT_NE(nullptr, model->FrameByIndex(3));
  EXPECT_EQ(nullptr, model->FrameByIndex(4));
  ASSERT_TRUE(model->FrameNameExists("F1"));
  ASSERT_TRUE(model->FrameNameExists("F2"));
  ASSERT_TRUE(model->FrameNameExists("F3"));
  ASSERT_TRUE(model->FrameNameExists("F4"));

  EXPECT_TRUE(model->FrameByName("F1")->AttachedTo().empty());
  EXPECT_TRUE(model->FrameByName("F2")->AttachedTo().empty());
  EXPECT_TRUE(model->FrameByName("F3")->AttachedTo().empty());
  EXPECT_TRUE(model->FrameByName("F4")->AttachedTo().empty());

  EXPECT_EQ("P", model->FrameByName("F1")->PoseRelativeTo());
  EXPECT_EQ("C", model->FrameByName("F2")->PoseRelativeTo());
  EXPECT_EQ("J", model->FrameByName("F3")->PoseRelativeTo());
  EXPECT_EQ("F3", model->FrameByName("F4")->PoseRelativeTo());
}

/////////////////////////////////////////////////
TEST(DOMFrame, LoadWorldFramesRelativeTo)
{
  const std::string testFile =
    sdf::filesystem::append(PROJECT_SOURCE_PATH, "test", "sdf",
        "world_frame_relative_to.sdf");

  // Load the SDF file
  sdf::Root root;
  EXPECT_TRUE(root.Load(testFile).empty());

  // Get the first world
  const sdf::World *world = root.WorldByIndex(0);
  ASSERT_NE(nullptr, world);
  EXPECT_EQ("world_frame_relative_to", world->Name());
  EXPECT_EQ(4u, world->ModelCount());
  EXPECT_NE(nullptr, world->ModelByIndex(0));
  EXPECT_NE(nullptr, world->ModelByIndex(1));
  EXPECT_NE(nullptr, world->ModelByIndex(2));
  EXPECT_NE(nullptr, world->ModelByIndex(3));
  EXPECT_EQ(nullptr, world->ModelByIndex(4));

  ASSERT_TRUE(world->ModelNameExists("M1"));
  ASSERT_TRUE(world->ModelNameExists("M2"));
  ASSERT_TRUE(world->ModelNameExists("M3"));
  ASSERT_TRUE(world->ModelNameExists("M4"));

  const sdf::Model *model = world->ModelByName("M1");
  ASSERT_NE(nullptr, model);
  EXPECT_EQ(1u, model->LinkCount());
  EXPECT_NE(nullptr, model->LinkByIndex(0));
  EXPECT_EQ(nullptr, model->LinkByIndex(1));
  EXPECT_EQ(1u, model->FrameCount());
  EXPECT_NE(nullptr, model->FrameByIndex(0));
  EXPECT_EQ(nullptr, model->FrameByIndex(1));
  ASSERT_TRUE(model->LinkNameExists("L"));
  ASSERT_TRUE(model->FrameNameExists("F0"));
  EXPECT_EQ("L", model->FrameByName("F0")->PoseRelativeTo());

  EXPECT_TRUE(world->ModelByName("M1")->PoseRelativeTo().empty());
  EXPECT_TRUE(world->ModelByName("M2")->PoseRelativeTo().empty());
  EXPECT_EQ("M2", world->ModelByName("M3")->PoseRelativeTo());
  EXPECT_EQ("F1", world->ModelByName("M4")->PoseRelativeTo());

  EXPECT_EQ(4u, world->FrameCount());
  EXPECT_NE(nullptr, world->FrameByIndex(0));
  EXPECT_NE(nullptr, world->FrameByIndex(1));
  EXPECT_NE(nullptr, world->FrameByIndex(2));
  EXPECT_NE(nullptr, world->FrameByIndex(3));
  EXPECT_EQ(nullptr, world->FrameByIndex(4));
  ASSERT_TRUE(world->FrameNameExists("world"));
  ASSERT_TRUE(world->FrameNameExists("F0"));
  ASSERT_TRUE(world->FrameNameExists("F1"));
  ASSERT_TRUE(world->FrameNameExists("F2"));

  EXPECT_TRUE(world->FrameByName("world")->PoseRelativeTo().empty());
  EXPECT_TRUE(world->FrameByName("F0")->PoseRelativeTo().empty());
  EXPECT_EQ("F0", world->FrameByName("F1")->PoseRelativeTo());
  EXPECT_EQ("M1", world->FrameByName("F2")->PoseRelativeTo());

  EXPECT_TRUE(world->FrameByName("world")->AttachedTo().empty());
  EXPECT_TRUE(world->FrameByName("F0")->AttachedTo().empty());
  EXPECT_TRUE(world->FrameByName("F1")->AttachedTo().empty());
  EXPECT_TRUE(world->FrameByName("F2")->AttachedTo().empty());
}

/////////////////////////////////////////////////
TEST(DOMFrame, LoadWorldFramesInvalidRelativeTo)
{
  const std::string testFile =
    sdf::filesystem::append(PROJECT_SOURCE_PATH, "test", "sdf",
        "world_frame_invalid_relative_to.sdf");

  // Load the SDF file
  sdf::Root root;
  EXPECT_TRUE(root.Load(testFile).empty());

  // Get the first world
  const sdf::World *world = root.WorldByIndex(0);
  ASSERT_NE(nullptr, world);
  EXPECT_EQ("world_frame_invalid_relative_to", world->Name());
  EXPECT_EQ(2u, world->ModelCount());
  EXPECT_NE(nullptr, world->ModelByIndex(0));
  EXPECT_NE(nullptr, world->ModelByIndex(1));
  EXPECT_EQ(nullptr, world->ModelByIndex(2));
  ASSERT_TRUE(world->ModelNameExists("cycle"));
  ASSERT_TRUE(world->ModelNameExists("M"));

  EXPECT_EQ("A", world->ModelByName("M")->PoseRelativeTo());
  EXPECT_EQ("cycle", world->ModelByName("cycle")->PoseRelativeTo());

  EXPECT_EQ(2u, world->FrameCount());
  EXPECT_NE(nullptr, world->FrameByIndex(0));
  EXPECT_NE(nullptr, world->FrameByIndex(1));
  EXPECT_EQ(nullptr, world->FrameByIndex(2));
  ASSERT_TRUE(world->FrameNameExists("self_cycle"));
  ASSERT_TRUE(world->FrameNameExists("F"));

  EXPECT_EQ("A", world->FrameByName("F")->PoseRelativeTo());
  EXPECT_EQ("self_cycle", world->FrameByName("self_cycle")->PoseRelativeTo());
}
