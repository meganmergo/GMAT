//$Id$
//------------------------------------------------------------------------------
//                                  Toggle
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Author: Darrel J. Conway
// Created: 2003/mm/dd
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Insert descriptive text here.
 *
 * @note Any notes here.
 */
//------------------------------------------------------------------------------


// Class automatically generated by Dev-C++ New Class wizard

#include "Toggle.hpp" // class's header file
#include "Publisher.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"

//#define DEBUG_RENAME
//#define DEBUG_TOGGLE_IA
//#define DEBUG_TOGGLE_SET
//#define DEBUG_TOGGLE_INIT
//#define DEBUG_TOGGLE_EXE
//#define DEBUG_TAKE_ACTION

// class constructor
//------------------------------------------------------------------------------
// Toggle()
//------------------------------------------------------------------------------
Toggle::Toggle() :
   GmatCommand    ("Toggle"),
   toggleState    (true),
   subscriberID   (parameterCount),
   toggleStateID  (++parameterCount)
{
   objectTypeNames.push_back("SubscriberCommand");
   ++parameterCount;
}

// class destructor
//------------------------------------------------------------------------------
// ~Toggle()
//------------------------------------------------------------------------------
Toggle::~Toggle()
{
   // insert your code here
}


//------------------------------------------------------------------------------
// Toggle(const Toggle& t)
//------------------------------------------------------------------------------
Toggle::Toggle(const Toggle& t) :
   GmatCommand        (t),
   toggleState        (t.toggleState),
   subscriberID       (t.subscriberID),
   toggleStateID      (t.toggleStateID)
{
   parameterCount = t.parameterCount;
   subNames = t.subNames;
   subs.clear();
}


//------------------------------------------------------------------------------
// Toggle& operator=(const Toggle& t)
//------------------------------------------------------------------------------
Toggle& Toggle::operator=(const Toggle& t)
{
   if (this == &t)
      return *this;
   
   GmatCommand::operator=(t);
   toggleState = t.toggleState;
   subscriberID = t.subscriberID;
   toggleStateID = t.toggleStateID;
   subNames = t.subNames;
   subs.clear();
   
   return *this;
}


//------------------------------------------------------------------------------
// bool InterpretAction()
//------------------------------------------------------------------------------
/**
 * Parses Toggle command string.
 * Sample string:  "Toggle Report On"
 */
//------------------------------------------------------------------------------
bool Toggle::InterpretAction()
{
   subNames.clear();
   subs.clear();
   
   std::string genStr = generatingString;
   
   // Trim it first
   genStr = GmatStringUtil::Trim(genStr, GmatStringUtil::BOTH, true, true);
   
   Integer loc = genStr.find("Toggle", 0) + 6;
   const char *str = genStr.c_str();
   while (str[loc] == ' ')
      ++loc;
   Integer subEnd, commentStart;
   if ((commentStart = (Integer) genStr.find_first_of("%", 0)) != 
       (Integer)genStr.npos)  
      subEnd = commentStart;
   else 
      subEnd = (Integer) genStr.size();
   std::string str1 = genStr.substr(loc, subEnd-loc);
   #ifdef DEBUG_TOGGLE_IA
      MessageInterface::ShowMessage("In InterpretAction, str1 = \n");
      MessageInterface::ShowMessage("   %s\n", str1.c_str());
   #endif

   // this command, for compatability with MATLAB, should not have
   // parentheses (except to indicate array elements), brackets, or braces
   if (!GmatStringUtil::HasNoBrackets(str, false))
   {
      std::string msg = 
         "The Toggle command is not allowed to contain brackets, braces, or "
         "parentheses";
      throw CommandException(msg);
   }
   StringArray parts = GmatStringUtil::SeparateBy(str1," ", false);
   Integer partsSz = (Integer) parts.size();
   #ifdef DEBUG_TOGGLE_IA
      MessageInterface::ShowMessage("In InterpretAction, parts = \n");
      for (Integer jj = 0; jj < partsSz; jj++)
         MessageInterface::ShowMessage("   %s\n", parts.at(jj).c_str());
   #endif
   if (partsSz < 2) // 'Toggle' already found
      throw CommandException("Missing field in Toggle command");
   if (parts.at(partsSz-1) == "On")       toggleState = true;
   else if (parts.at(partsSz-1) == "Off") toggleState = false;
   else
      throw CommandException("Missing or misplaced 'On' or 'Off' in Toggle command");
   for (Integer ii = 0; ii < partsSz-1; ii++)
   {
      if ((parts.at(ii) == "On") || (parts.at(ii) == "Off"))
         throw CommandException("Too many 'On's or 'Off's in Toggle command");
      #ifdef DEBUG_TOGGLE_IA
         MessageInterface::ShowMessage("Adding subName %s \n", parts.at(ii).c_str());
      #endif
               
      subNames.push_back(parts.at(ii));
   }
   
   return true;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool Toggle::Initialize()
{
   #ifdef DEBUG_TOGGLE_INIT
      MessageInterface::ShowMessage("Toggle::Initialize() entered\n");
   #endif
      
   GmatCommand::Initialize();
   BuildSubscriberList();
   return true;
}


//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
bool Toggle::Execute()
{
   #ifdef DEBUG_TOGGLE_EXE
   MessageInterface::ShowMessage
      ("Toggle::Execute() entered, There are %d subscriber(s)\n", subNames.size());
   #ifdef DEBUG_TOGGLE_EXE_MAP
   ShowObjectMaps("In Toggle::Execute()");
   #endif
   #endif
   
   if (currentFunction != NULL)
   {
      #ifdef DEBUG_TOGGLE_EXE
      MessageInterface::ShowMessage
         ("   Inside a function, so about to refresh subscriber pointers...\n");
      #endif
      
      // We need to refresh subscriber pointers here, since the
      // CcsdsEphemerisFile is created and replaces old EphemerisFile in the
      // object map
      BuildSubscriberList();
   }
   
   for (std::list<Subscriber *>::iterator s = subs.begin(); s != subs.end(); ++s)
   {
      #ifdef DEBUG_TOGGLE_EXE
         MessageInterface::ShowMessage("Toggle::Execute() calling "
               "%s->Activate(%s)\n", (*s)->GetName().c_str(),
               (toggleState ? "true" : "false"));
      #endif
      
      (*s)->Activate(toggleState);
      if (toggleState == true)
         (*s)->TakeAction("ToggleOn");
      else
         (*s)->TakeAction("ToggleOff");
   }
   
   // Why do we need this? (loj: 2008.08.01)
   // This causes an error if Toggle is used in the GmatFunction, so commented out
   // Publisher Exception: Character data provider has not registered with the Publisher.
   //char data[] = "Toggle executed\n\n";
   //publisher->Publish(streamID, data, strlen(data));
   
   BuildCommandSummary(true);
   
   #ifdef DEBUG_TOGGLE_EXE
      MessageInterface::ShowMessage("Toggle::Execute() leaving\n");
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Toggle.
 *
 * @return clone of the Toggle.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Toggle::Clone() const
{
   return (new Toggle(*this));
}


//------------------------------------------------------------------------------
// virtual bool TakeAction(const std::string &action,  
//                         const std::string &actionData = "")
//------------------------------------------------------------------------------
/**
 * This method performs action.
 *
 * @param <action> action to perform
 * @param <actionData> action data associated with action
 *
 * @return true if action successfully performed
 */
//------------------------------------------------------------------------------
bool Toggle::TakeAction(const std::string &action, const std::string &actionData)
{
   #ifdef DEBUG_TAKE_ACTION
   MessageInterface::ShowMessage
      ("Toggle::TakeAction() entered, action=%s, actionData=%s, subNames.size=%d, "
       "subs.size=%d\n", action.c_str(), actionData.c_str(), subNames.size(), subs.size());
   #endif
   
   if (action == "Clear")
   {
      subNames.clear();
      subs.clear();
   }
   
   #ifdef DEBUG_TAKE_ACTION
   MessageInterface::ShowMessage
      ("PlotCommand::TakeAction() returning GmatCommand::TakeAction()\n");
   #endif
   
   return GmatCommand::TakeAction(action, actionData);
}


//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref object types used by the Toggle.
 *
 * @return the list of object types.
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& Toggle::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::SUBSCRIBER);

   return refObjectTypes;
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Accesses arrays of names for referenced objects.
 * 
 * @param type Type of object requested.
 * 
 * @return the StringArray containing the referenced object names.
 */
//------------------------------------------------------------------------------
const StringArray& Toggle::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   // There are only subscribers, so ignore object type
   return subNames;
}


//------------------------------------------------------------------------------
// bool IncludeOwnedObjectsInValidation()
//------------------------------------------------------------------------------
/**
 * Indicates if owned objects are acceptable proxies during validation.
 *
 * This method is used to indicate that an object accepts referenced objects
 * that own a specified object type in addition to objects of that type.
 *
 * @return true, since owned objects are included in the type validation of a
 *         script line for the Toggle command.
 */
//------------------------------------------------------------------------------
bool Toggle::IncludeOwnedObjectsInValidation()
{
   return true;
}


//------------------------------------------------------------------------------
//  const std::string GetGeneratingString()
//------------------------------------------------------------------------------
/**
 * Method used to retrieve the string that was parsed to build this GmatCommand.
 *
 * This method is used to retrieve the GmatCommand string from the script that
 * was parsed to build the GmatCommand.  It is used to save the script line, so
 * that the script can be written to a file without inverting the steps taken to
 * set up the internal object data.  As a side benefit, the script line is
 * available in the GmatCommand structure for debugging purposes.
 *
 * @param <mode>    Specifies the type of serialization requested.
 * @param <prefix>  Optional prefix appended to the object's name. (Used for
 *                  indentation)
 * @param <useName> Name that replaces the object's name (Not yet used
 *                  in commands).
 *
 * @return The script line that defines this GmatCommand.
 */
//------------------------------------------------------------------------------
const std::string& Toggle::GetGeneratingString(Gmat::WriteMode mode,
                                               const std::string &prefix,
                                               const std::string &useName)
{
   generatingString = prefix + "Toggle ";
   for (StringArray::iterator i = subNames.begin(); i != subNames.end(); ++i)
      generatingString += (*i) + " ";
   generatingString += ((toggleState == true) ? "On;" : "Off;");
   return GmatCommand::GetGeneratingString(mode, prefix, useName);
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool Toggle::RenameRefObject(const Gmat::ObjectType type,
                             const std::string &oldName,
                             const std::string &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Toggle::RenameConfiguredItem() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif
   
   if (type != Gmat::SUBSCRIBER)
      return true;

   for (unsigned int i=0; i<subNames.size(); i++)
   {
      if (subNames[i] == oldName)
         subNames[i] = newName;
   }

   return true;
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string Toggle::GetParameterText(const Integer id) const
{
   if (id == subscriberID)
      return "Subscriber";
   else if (id == toggleStateID)
      return "ToggleState";
   return GmatCommand::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer Toggle::GetParameterID(const std::string &str) const
{
   if (str == "Subscriber")
      return subscriberID;
   else if (str == "ToggleState")
      return toggleStateID;
   return GmatCommand::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType Toggle::GetParameterType(const Integer id) const
{
   if (id == subscriberID)
      return Gmat::STRING_TYPE;
   return GmatCommand::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string Toggle::GetParameterTypeString(const Integer id) const
{
   if (id == subscriberID)
      return PARAM_TYPE_STRING[Gmat::STRING_TYPE];
   return GmatCommand::GetParameterTypeString(id);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string Toggle::GetStringParameter(const Integer id) const
{
   if (id == toggleStateID)
   {
      if (toggleState == true)
         return "On";
      else
         return "Off";
   }
   
   return GmatCommand::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool Toggle::SetStringParameter(const Integer id, const std::string &value)
{

   #ifdef DEBUG_TOGGLE_SET
   MessageInterface::ShowMessage
      ("Toggle::SetStringParameter() id=%d, value=%s\n", id, value.c_str());
   #endif

   if (value == "")
      return false;
   
   if (id == subscriberID)
   {
      if (find(subNames.begin(), subNames.end(), value) == subNames.end())
         subNames.push_back(value);
      
      return true;
   }
   else if (id == toggleStateID)
   {
      if (value == "On")
         toggleState = true;
      else
         toggleState = false;

      return true;
   }
   
   return GmatCommand::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
std::string Toggle::GetStringParameter(const Integer id,
                                       const Integer index) const
{
   if (id == subscriberID)
   {
      if (index < 0 || index >= (Integer) subNames.size())
         throw CommandException(
                  "Index out-of-range for subscriber names list for Toggle command.\n");
      return subNames.at(index); 
   }
   return GmatCommand::GetStringParameter(id, index);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value,
//                         const Integer index)
//------------------------------------------------------------------------------
bool Toggle::SetStringParameter(const Integer id, 
                                const std::string &value,
                                const Integer index)
{
   if (value == "")
      return false;
   
   if (id == subscriberID)
   {
      if (index < 0 || index > (Integer) subNames.size())
         throw CommandException(
                  "Index out-of-range for subscriber names list for Toggle command.\n");
      else
      {
         if (index == (Integer) subNames.size())  subNames.push_back(value);
         else                   subNames.at(index) = value;
         
         //@todo Do we need this code? Commented out for now (LOJ: 2009.06.01)
         //if (publisher == NULL)
         //   publisher = Publisher::Instance();         
         //streamID = publisher->RegisterPublishedData(this, subNames, subNames);
      }
      
      return true;
   }
   return GmatCommand::SetStringParameter(id, value, index);
}


//------------------------------------------------------------------------------
// Protected methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// void Toggle::BuildSubscriberList()
//------------------------------------------------------------------------------
/**
 * Fills in the object array of subscribers
 */
//------------------------------------------------------------------------------
void Toggle::BuildSubscriberList()
{
   Subscriber *sub;
   subs.clear();
   GmatBase *mapObj = NULL;

   for (StringArray::iterator s = subNames.begin(); s != subNames.end(); ++s)
   {
      if ((mapObj = FindObject(*s)) != NULL)
      {
         if (mapObj->IsOfType(Gmat::SUBSCRIBER))
         {
            sub = (Subscriber *)mapObj;
            if (sub)
            {
               subs.push_back(sub);
            }
         }
         else // Handle owned Subscribers
         {
            Integer count = mapObj->GetOwnedObjectCount();

            if (count > 0)
            {
               for (Integer i = 0; i < count; ++i)
               {
                  GmatBase *oo = mapObj->GetOwnedObject(i);
                  if (oo->IsOfType(Gmat::SUBSCRIBER))
                  {
                     sub = (Subscriber *)oo;
                     subs.push_back(sub);
                  }
               }
               // Tell owning object that subscriber may have changed state
               mapObj->TakeAction("CheckSubscribers");
            }
            else
               MessageInterface::ShowMessage("Toggle command cannot find "
                     "any subscriber services for object %s\n", s->c_str());
         }
      }
      else
      {
         MessageInterface::ShowMessage
            ("Toggle command cannot find subscriber %s; command has no effect "
                  "for that object\n", s->c_str());
      }
   }

   //@todo Do we need this code? Commented out for now (LOJ: 2009.06.01)
   //if (publisher == NULL)
   //   publisher = Publisher::Instance();
   //streamID = publisher->RegisterPublishedData(this, subNames, subNames);

   #ifdef DEBUG_TOGGLE_INIT
      MessageInterface::ShowMessage("There are %d subscriber(s)\n",
            subs.size());
      for (std::list<Subscriber *>::iterator s = subs.begin();
            s != subs.end(); ++s)
      {
         MessageInterface::ShowMessage
            ("   subscriber = <%p><%s>'%s'\n", *s,
             (*s)->GetTypeName().c_str(), (*s)->GetName().c_str());
      }
      MessageInterface::ShowMessage("Toggle::Initialize() leaving\n");
   #endif
}
