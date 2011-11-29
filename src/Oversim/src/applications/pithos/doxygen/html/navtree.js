var NAVTREE =
[
  [ "Pithos", "index.html", [
    [ "Class List", "annotated.html", [
      [ "bootstrapPkt", "classbootstrap_pkt.html", null ],
      [ "bootstrapPktDescriptor", "classbootstrap_pkt_descriptor.html", null ],
      [ "Communicator", "class_communicator.html", null ],
      [ "DHTStorage::DHTStatsContext", "class_d_h_t_storage_1_1_d_h_t_stats_context.html", null ],
      [ "DHTStorage", "class_d_h_t_storage.html", null ],
      [ "Directory_logic", "class_directory__logic.html", null ],
      [ "Game", "class_game.html", null ],
      [ "GameObject", "class_game_object.html", null ],
      [ "groupPkt", "classgroup_pkt.html", null ],
      [ "groupPktDescriptor", "classgroup_pkt_descriptor.html", null ],
      [ "GroupStorage", "class_group_storage.html", null ],
      [ "Message", "class_message.html", null ],
      [ "MessageDescriptor", "class_message_descriptor.html", null ],
      [ "ObjectInfo", "class_object_info.html", null ],
      [ "overlayPkt", "classoverlay_pkt.html", null ],
      [ "overlayPktDescriptor", "classoverlay_pkt_descriptor.html", null ],
      [ "OverlayStorage", "class_overlay_storage.html", null ],
      [ "Packet", "class_packet.html", null ],
      [ "PacketDescriptor", "class_packet_descriptor.html", null ],
      [ "Peer_logic", "class_peer__logic.html", null ],
      [ "PeerData", "class_peer_data.html", null ],
      [ "PeerListPkt", "class_peer_list_pkt.html", null ],
      [ "PeerListPkt_Base", "class_peer_list_pkt___base.html", null ],
      [ "PeerListPktDescriptor", "class_peer_list_pkt_descriptor.html", null ],
      [ "Queue", "class_queue.html", null ],
      [ "SP_element", "class_s_p__element.html", null ],
      [ "Storage", "class_storage.html", null ],
      [ "Super_peer_logic", "class_super__peer__logic.html", null ]
    ] ],
    [ "Class Index", "classes.html", null ],
    [ "Class Hierarchy", "hierarchy.html", [
      [ "bootstrapPktDescriptor", "classbootstrap_pkt_descriptor.html", null ],
      [ "Communicator", "class_communicator.html", null ],
      [ "DHTStorage::DHTStatsContext", "class_d_h_t_storage_1_1_d_h_t_stats_context.html", null ],
      [ "DHTStorage", "class_d_h_t_storage.html", null ],
      [ "Directory_logic", "class_directory__logic.html", null ],
      [ "Game", "class_game.html", null ],
      [ "GameObject", "class_game_object.html", null ],
      [ "groupPktDescriptor", "classgroup_pkt_descriptor.html", null ],
      [ "GroupStorage", "class_group_storage.html", null ],
      [ "Message", "class_message.html", null ],
      [ "MessageDescriptor", "class_message_descriptor.html", null ],
      [ "ObjectInfo", "class_object_info.html", null ],
      [ "overlayPkt", "classoverlay_pkt.html", null ],
      [ "overlayPktDescriptor", "classoverlay_pkt_descriptor.html", null ],
      [ "OverlayStorage", "class_overlay_storage.html", null ],
      [ "Packet", "class_packet.html", [
        [ "bootstrapPkt", "classbootstrap_pkt.html", null ],
        [ "groupPkt", "classgroup_pkt.html", null ],
        [ "PeerListPkt_Base", "class_peer_list_pkt___base.html", [
          [ "PeerListPkt", "class_peer_list_pkt.html", null ]
        ] ]
      ] ],
      [ "PacketDescriptor", "class_packet_descriptor.html", null ],
      [ "Peer_logic", "class_peer__logic.html", null ],
      [ "PeerData", "class_peer_data.html", null ],
      [ "PeerListPktDescriptor", "class_peer_list_pkt_descriptor.html", null ],
      [ "Queue", "class_queue.html", null ],
      [ "SP_element", "class_s_p__element.html", null ],
      [ "Storage", "class_storage.html", null ],
      [ "Super_peer_logic", "class_super__peer__logic.html", null ]
    ] ],
    [ "Class Members", "functions.html", null ],
    [ "File List", "files.html", [
      [ "bootstrapPkt_m.h", null, null ],
      [ "Communicator.h", null, null ],
      [ "DHTStorage.h", null, null ],
      [ "Directory_logic.h", null, null ],
      [ "Game.h", null, null ],
      [ "GameObject.h", null, null ],
      [ "groupPkt_m.h", null, null ],
      [ "GroupStorage.h", null, null ],
      [ "Message_m.h", null, null ],
      [ "ObjectInfo.h", null, null ],
      [ "overlayPkt_m.h", null, null ],
      [ "OverlayStorage.h", null, null ],
      [ "packet_m.h", null, null ],
      [ "Peer_logic.h", null, null ],
      [ "PeerData.h", null, null ],
      [ "PeerListPkt.h", null, null ],
      [ "PeerListPkt_m.h", null, null ],
      [ "Queue.h", null, null ],
      [ "SP_element.h", null, null ],
      [ "Storage.h", null, null ],
      [ "Super_peer_logic.h", null, null ]
    ] ]
  ] ]
];

function createIndent(o,domNode,node,level)
{
  if (node.parentNode && node.parentNode.parentNode)
  {
    createIndent(o,domNode,node.parentNode,level+1);
  }
  var imgNode = document.createElement("img");
  if (level==0 && node.childrenData)
  {
    node.plus_img = imgNode;
    node.expandToggle = document.createElement("a");
    node.expandToggle.href = "javascript:void(0)";
    node.expandToggle.onclick = function() 
    {
      if (node.expanded) 
      {
        $(node.getChildrenUL()).slideUp("fast");
        if (node.isLast)
        {
          node.plus_img.src = node.relpath+"ftv2plastnode.png";
        }
        else
        {
          node.plus_img.src = node.relpath+"ftv2pnode.png";
        }
        node.expanded = false;
      } 
      else 
      {
        expandNode(o, node, false);
      }
    }
    node.expandToggle.appendChild(imgNode);
    domNode.appendChild(node.expandToggle);
  }
  else
  {
    domNode.appendChild(imgNode);
  }
  if (level==0)
  {
    if (node.isLast)
    {
      if (node.childrenData)
      {
        imgNode.src = node.relpath+"ftv2plastnode.png";
      }
      else
      {
        imgNode.src = node.relpath+"ftv2lastnode.png";
        domNode.appendChild(imgNode);
      }
    }
    else
    {
      if (node.childrenData)
      {
        imgNode.src = node.relpath+"ftv2pnode.png";
      }
      else
      {
        imgNode.src = node.relpath+"ftv2node.png";
        domNode.appendChild(imgNode);
      }
    }
  }
  else
  {
    if (node.isLast)
    {
      imgNode.src = node.relpath+"ftv2blank.png";
    }
    else
    {
      imgNode.src = node.relpath+"ftv2vertline.png";
    }
  }
  imgNode.border = "0";
}

function newNode(o, po, text, link, childrenData, lastNode)
{
  var node = new Object();
  node.children = Array();
  node.childrenData = childrenData;
  node.depth = po.depth + 1;
  node.relpath = po.relpath;
  node.isLast = lastNode;

  node.li = document.createElement("li");
  po.getChildrenUL().appendChild(node.li);
  node.parentNode = po;

  node.itemDiv = document.createElement("div");
  node.itemDiv.className = "item";

  node.labelSpan = document.createElement("span");
  node.labelSpan.className = "label";

  createIndent(o,node.itemDiv,node,0);
  node.itemDiv.appendChild(node.labelSpan);
  node.li.appendChild(node.itemDiv);

  var a = document.createElement("a");
  node.labelSpan.appendChild(a);
  node.label = document.createTextNode(text);
  a.appendChild(node.label);
  if (link) 
  {
    a.href = node.relpath+link;
  } 
  else 
  {
    if (childrenData != null) 
    {
      a.className = "nolink";
      a.href = "javascript:void(0)";
      a.onclick = node.expandToggle.onclick;
      node.expanded = false;
    }
  }

  node.childrenUL = null;
  node.getChildrenUL = function() 
  {
    if (!node.childrenUL) 
    {
      node.childrenUL = document.createElement("ul");
      node.childrenUL.className = "children_ul";
      node.childrenUL.style.display = "none";
      node.li.appendChild(node.childrenUL);
    }
    return node.childrenUL;
  };

  return node;
}

function showRoot()
{
  var headerHeight = $("#top").height();
  var footerHeight = $("#nav-path").height();
  var windowHeight = $(window).height() - headerHeight - footerHeight;
  navtree.scrollTo('#selected',0,{offset:-windowHeight/2});
}

function expandNode(o, node, imm)
{
  if (node.childrenData && !node.expanded) 
  {
    if (!node.childrenVisited) 
    {
      getNode(o, node);
    }
    if (imm)
    {
      $(node.getChildrenUL()).show();
    } 
    else 
    {
      $(node.getChildrenUL()).slideDown("fast",showRoot);
    }
    if (node.isLast)
    {
      node.plus_img.src = node.relpath+"ftv2mlastnode.png";
    }
    else
    {
      node.plus_img.src = node.relpath+"ftv2mnode.png";
    }
    node.expanded = true;
  }
}

function getNode(o, po)
{
  po.childrenVisited = true;
  var l = po.childrenData.length-1;
  for (var i in po.childrenData) 
  {
    var nodeData = po.childrenData[i];
    po.children[i] = newNode(o, po, nodeData[0], nodeData[1], nodeData[2],
        i==l);
  }
}

function findNavTreePage(url, data)
{
  var nodes = data;
  var result = null;
  for (var i in nodes) 
  {
    var d = nodes[i];
    if (d[1] == url) 
    {
      return new Array(i);
    }
    else if (d[2] != null) // array of children
    {
      result = findNavTreePage(url, d[2]);
      if (result != null) 
      {
        return (new Array(i).concat(result));
      }
    }
  }
  return null;
}

function initNavTree(toroot,relpath)
{
  var o = new Object();
  o.toroot = toroot;
  o.node = new Object();
  o.node.li = document.getElementById("nav-tree-contents");
  o.node.childrenData = NAVTREE;
  o.node.children = new Array();
  o.node.childrenUL = document.createElement("ul");
  o.node.getChildrenUL = function() { return o.node.childrenUL; };
  o.node.li.appendChild(o.node.childrenUL);
  o.node.depth = 0;
  o.node.relpath = relpath;

  getNode(o, o.node);

  o.breadcrumbs = findNavTreePage(toroot, NAVTREE);
  if (o.breadcrumbs == null)
  {
    o.breadcrumbs = findNavTreePage("index.html",NAVTREE);
  }
  if (o.breadcrumbs != null && o.breadcrumbs.length>0)
  {
    var p = o.node;
    for (var i in o.breadcrumbs) 
    {
      var j = o.breadcrumbs[i];
      p = p.children[j];
      expandNode(o,p,true);
    }
    p.itemDiv.className = p.itemDiv.className + " selected";
    p.itemDiv.id = "selected";
    $(window).load(showRoot);
  }
}

