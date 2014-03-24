//
//  GRTTreeDataSource.mm
//  MySQLWorkbench
//
//  Created by Alfredo Kojima on 29/Sep/08.
//  Copyright 2008 Sun Microsystems Inc. All rights reserved.
//

#import "GRTTreeDataSource.h"
#import "GRTIconCache.h"
#import "MTextImageCell.h"
 
@implementation GRTTreeDataSource

- (id)initWithTreeModel:(bec::TreeModel*)model
{
  if ((self = [super init]) != nil)
  {
    _tree= model;
    _hideRootNode= NO;
    _nodeCache= [[NSMutableDictionary alloc] init];
  }
  return self;
}


- (id)init
{
  if ((self = [super init]) != nil)
  {
    _hideRootNode= NO;
    _nodeCache= [[NSMutableDictionary alloc] init];
  }
  return self;
}


- (void)dealloc
{
  [_normalFont release];
  [_boldFont release];
  [_nodeCache release];
  [super dealloc];
}

- (void)setTreeModel:(bec::TreeModel*)model
{
  _tree= model;
}

- (void)setHidesRootNode:(BOOL)flag
{
  _hideRootNode= flag;
}


- (bec::TreeModel*)treeModel
{
  return _tree;
}


- (void)refreshModel
{
  [_nodeCache removeAllObjects];
  
  _tree->refresh();
}


- (bec::NodeId)nodeIdForItem:(id)item
{
  if (item)
    return [item nodeId];
  else
  {
    if (_hideRootNode)
      return bec::NodeId(0);
    else
      return bec::NodeId();
  }
}


- (id)itemForNodeId:(const bec::NodeId&)nodeId
{
  NSString *key= [NSString stringWithUTF8String:nodeId.repr().c_str()];
  GRTNodeId *node;
  if (!(node= [_nodeCache objectForKey:key]))
  {
    node= [[GRTNodeId nodeIdWithNodeId:nodeId] retain];
    [_nodeCache setObject:node forKey:key];
  }
  return node;
}


- (id)outlineView:(NSOutlineView *)outlineView child:(NSInteger)index ofItem:(id)item
{
  try
  {
    return [[self itemForNodeId: _tree->get_child([self nodeIdForItem:item], index)] retain];
  }
  catch (const std::exception &e)
  {
    return nil;
  }
}


- (BOOL)outlineView:(NSOutlineView *)outlineView isItemExpandable:(id)item
{
  BOOL flag;
  try
  {
    flag=  !item || _tree->is_expandable([self nodeIdForItem:item]);
  }
  catch (...)
  {
    std::string s;
    _tree->get_field(_tree->get_parent([self nodeIdForItem:item]), 0, s);
  }
  return flag;
}


- (void)outlineViewItemWillExpand:(NSNotification *)notification
{
  id item= [[notification userInfo] objectForKey:@"NSObject"];
  if (_tree)
    _tree->expand_node([self nodeIdForItem:item]);
}


- (void)outlineViewItemDidCollapse:(NSNotification *)notification
{
  id item= [[notification userInfo] objectForKey:@"NSObject"];
  
  if (_tree)
    _tree->collapse_node([self nodeIdForItem:item]);
}


- (NSInteger)outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(id)item
{
  if (_tree)
    return _tree->count_children([self nodeIdForItem:item]);
  return 0;
}


- (id)outlineView:(NSOutlineView *)outlineView objectValueForTableColumn:(NSTableColumn *)tableColumn byItem:(id)item
{
  std::string strvalue;
  int column= [[tableColumn identifier] integerValue];

  if (!_tree || !_tree->get_field([self nodeIdForItem:item], column, strvalue))
    return nil;
  
  return [NSString stringWithUTF8String:strvalue.c_str()];
}


- (void)outlineView:(NSOutlineView *)outlineView setObjectValue:(id)object forTableColumn:(NSTableColumn *)tableColumn byItem:(id)item
{
  int column= [[tableColumn identifier] integerValue];
  
  if ([object respondsToSelector:@selector(UTF8String)])
    _tree->set_field([self nodeIdForItem:item], column, [object UTF8String]);
  else if ([object respondsToSelector:@selector(integerValue)])
    _tree->set_field([self nodeIdForItem:item], column, (int)[object integerValue]);
}


- (void)outlineView:(NSOutlineView *)outlineView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn item:(id)item
{
  if ([cell isKindOfClass:[MTextImageCell class]])
  {
    bec::NodeId node_id= [self nodeIdForItem:item];
    bec::IconId icon_id= 0;
    if (_tree != nil)
      icon_id= _tree->get_field_icon(node_id, [[tableColumn identifier] integerValue], bec::Icon16);
    
    NSImage *image= [[GRTIconCache sharedIconCache] imageForIconId:icon_id];
    
    if (icon_id != 0 && !image && _tree->is_expandable(node_id))
    {
      image= [[GRTIconCache sharedIconCache] imageForFolder:bec::Icon16];
    }

    if (icon_id != 0)
      [cell setImage:image];
    else
      [cell setImage:nil];

    {
      if (!_normalFont)
      {
        _normalFont = [[NSFont systemFontOfSize: [[cell font] pointSize]] retain];
        _boldFont = [[NSFont boldSystemFontOfSize: [_normalFont pointSize]] retain];
      }
      
      if (_tree && _tree->is_highlighted(node_id))
        [cell setFont: _boldFont];
      else
        [cell setFont: _normalFont];
    }
  }
}


- (void)setDragDelegate:(id<GRTDragDelegate>)delegate
{
  _dragDelegate= delegate;
}


- (BOOL)outlineView:(NSOutlineView *)outlineView writeItems:(NSArray *)items toPasteboard:(NSPasteboard *)pboard
{
  if (_dragDelegate)
    return [_dragDelegate dataSource:self
                          writeItems:items
                        toPasteboard:pboard];
  
  return NO;
}


static void restore_expanded_child_nodes(NSMutableSet *mset,
                                         GRTTreeDataSource *ds,
                                         NSOutlineView *outlineView,
                                         id column,
                                         NSString *prefix,
                                         id node)
{
  if ([mset containsObject: prefix])
  {
    [outlineView collapseItem: node];
    [ds treeModel]->expand_node([ds nodeIdForItem: node]);
    [outlineView expandItem: node];
  }
  
  for (int c = [ds outlineView:outlineView numberOfChildrenOfItem:node], i = 0; i < c; i++)
  {
    id child = [ds outlineView:outlineView child:i ofItem:node];
    NSString *suffix = [ds outlineView:outlineView objectValueForTableColumn:column byItem:child];
    if (suffix)
    {
      NSString *childPrefix = [NSString stringWithFormat: @"%@/%@", prefix, suffix];
      restore_expanded_child_nodes(mset, ds, outlineView, column, 
                                   childPrefix, 
                                   child);
    }
  }
}


- (void)restoreExpansionStateOfOutlineView:(NSOutlineView*)outlineView
                                 fromState:(NSMutableSet*)state
                        usingValueOfColumn:(id)column
{
  restore_expanded_child_nodes(state, self, outlineView, 
                               [outlineView tableColumnWithIdentifier:column], @"", nil);
}


static void save_expanded_child_nodes(NSMutableSet *mset,
                                      GRTTreeDataSource *ds,
                                      NSOutlineView *outlineView,
                                      id column,
                                      NSString *prefix,
                                      id node)
{
  if ([outlineView isItemExpanded: node])
    [mset addObject: prefix];
  
  for (int c = [ds outlineView:outlineView numberOfChildrenOfItem:node], i = 0; i < c; i++)
  {
    id child = [ds outlineView:outlineView child:i ofItem:node];
    NSString *suffix = [ds outlineView:outlineView objectValueForTableColumn:column byItem:child];
    if (suffix)
    {
      NSString *childPrefix = [NSString stringWithFormat: @"%@/%@", prefix, suffix];
      save_expanded_child_nodes(mset, ds, outlineView, column, 
                                childPrefix, 
                                child);
    }
  }
}

- (NSMutableSet*)storeExpansionStateOfOutlineView:(NSOutlineView*)outlineView
                               usingValueOfColumn:(id)column
{
  NSMutableSet *mset = [NSMutableSet set];
  
  save_expanded_child_nodes(mset, self, outlineView, 
                            [outlineView tableColumnWithIdentifier:column], @"", nil);
  return mset;
}

@end
