/* 
 * Copyright (c) 2009, 2013, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the
 * License.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */

#import <Cocoa/Cocoa.h>

#import "WBPluginEditorBase.h"

#include "wb_editor_storednote.h"

@class MFCodeEditor;

@interface StoredNoteEditor : WBPluginEditorBase {
  IBOutlet NSTabView *tabView; // this editor has a single Tab, but we put in a TabView for homegeneity
  
  IBOutlet NSView* editorHost;

  @private
  StoredNoteEditorBE *mBackEnd;
}

- (id)initWithModule:(grt::Module*)module GRTManager:(bec::GRTManager*)grtm arguments:(const grt::BaseListRef&)args;

- (IBAction)applyChanges:(id)sender;
- (IBAction)revertChanges:(id)sender;

- (bec::BaseEditor*) editorBE;

@end
