//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//


package oversim.underlay.simpleunderlay;

import inet.base.NotificationBoard;
import inet.networklayer.common.InterfaceTable;
import oversim.common.BootstrapList;
import oversim.common.CryptoModule;
import oversim.common.IOverlay;
import oversim.common.ITier;
import oversim.common.ITier_overlay;
import oversim.common.NeighborCache;


//
// Host in the simple network that participates in the overlay
//
// @author John Gilmore, Stephan Krause, Bernhard Heep, Ingmar Baumgart
//
module MMVEHost
{
    parameters:
        string overlayType; // overlay protocol compound module to use
        string agentType;
        string eventLayerIMType;
        string eventDisseminationType;
        string eventOrderingType;
        string gameLogicType;
        string rootObjectUpdaterType;
        string rootObjectStoreType;
        string overlayStorageType;
        string updateLayerIMType;
        string updateDisseminationType;
        string localObjectUpdaterType;
        string localObjectStoreType;
        string displayUpdaterType;

        @display("bgb=700,500;i=device/wifilaptop_l;i2=block/circle_s");
    gates:
        input overlayNeighborArrowIn[]; // incoming gate for visualizing overlay neighborship with connection arrows
        output overlayNeighborArrowOut[]; // incoming gate for visualizing overlay neighborship with connection arrows

    submodules:
        agent: <agentType> like ITier {
            parameters:
                @display("p=79,47;i=block/segm");
        }
        eventLayerIM: <eventLayerIMType> like ITier {
            parameters:
                @display("p=202,47;i=block/segm");
        }
        eventDissemination: <eventDisseminationType> like ITier {
            parameters:
                @display("p=349,47;i=block/segm");
        }
        eventOrdering: <eventOrderingType> like ITier {
            parameters:
                @display("p=477,47;i=block/segm");
        }
        gameLogic: <gameLogicType> like ITier {
            parameters:
                @display("p=617,47;i=block/segm");
        }
        rootObjectUpdater: <rootObjectUpdaterType> like ITier {
            parameters:
                @display("p=617,149;i=block/segm");
        }
        rootObjectStore: <rootObjectStoreType> like ITier_overlay {
            parameters:
                @display("p=477,149;i=block/segm");
        }
        overlayStorage: <overlayStorageType> like ITier {
            parameters:
                @display("p=477,290;i=block/segm");
        }
        updateLayerIM: <updateLayerIMType> like ITier {
            parameters:
                @display("p=349,149;i=block/segm");
        }
        updateDissemination: <updateDisseminationType> like ITier {
            parameters:
                @display("p=202,149;i=block/segm");
        }
        objectMerger: <updateDisseminationType> like ITier {
            parameters:
                @display("p=79,149;i=block/segm");
        }
        localObjectUpdater: <localObjectUpdaterType> like ITier {
            parameters:
                @display("p=79,226;i=block/segm");
        }
        localObjectStore: <localObjectStoreType> like ITier {
            parameters:
                @display("p=349,226;i=block/segm");
        }
        displayUpdater: <displayUpdaterType> like ITier {
            parameters:
                @display("p=617,226;i=block/segm");
        }
        overlay: <overlayType> like IOverlay {
            parameters:
                @display("p=477,373;i=block/network2");
        }
        udp: SimpleUDP {
            parameters:
                @display("p=349,446");
        }
        tcp: SimpleTCP {
            parameters:
                @display("p=202,446");
        }
        cryptoModule: CryptoModule {
            parameters:
                @display("p=79,373");
        }
        notificationBoard: NotificationBoard {
            parameters:
                @display("p=79,446");
        }
        interfaceTable: InterfaceTable {
            parameters:
                @display("p=617,373");
        }
        neighborCache: NeighborCache {
            parameters:
                @display("p=617,446");
        }
        bootstrapList: BootstrapList {
            parameters:
                @display("p=477,446");
        }
    connections allowunconnected:
        agent.udpOut --> udp.appIn++;
        udp.appOut++ --> agent.udpIn;
        agent.tcpOut --> tcp.appIn++;
        tcp.appOut++ --> agent.tcpIn;

        agent.to_lowerTier --> eventLayerIM.from_upperTier;
        agent.from_lowerTier <-- eventLayerIM.to_upperTier;
        eventLayerIM.udpOut --> udp.appIn++;
        udp.appOut++ --> eventLayerIM.udpIn;
        eventLayerIM.tcpOut --> tcp.appIn++;
        tcp.appOut++ --> eventLayerIM.tcpIn;

        eventLayerIM.to_lowerTier --> eventDissemination.from_upperTier;
        eventLayerIM.from_lowerTier <-- eventDissemination.to_upperTier;
        eventDissemination.udpOut --> udp.appIn++;
        udp.appOut++ --> eventDissemination.udpIn;
        eventDissemination.tcpOut --> tcp.appIn++;
        tcp.appOut++ --> eventDissemination.tcpIn;

        eventDissemination.to_lowerTier --> eventOrdering.from_upperTier;
        eventDissemination.from_lowerTier <-- eventOrdering.to_upperTier;
        eventOrdering.udpOut --> udp.appIn++;
        udp.appOut++ --> eventOrdering.udpIn;
        eventOrdering.tcpOut --> tcp.appIn++;
        tcp.appOut++ --> eventOrdering.tcpIn;

        eventOrdering.to_lowerTier --> gameLogic.from_upperTier;
        eventOrdering.from_lowerTier <-- gameLogic.to_upperTier;
        gameLogic.udpOut --> udp.appIn++;
        udp.appOut++ --> gameLogic.udpIn;
        gameLogic.tcpOut --> tcp.appIn++;
        tcp.appOut++ --> gameLogic.tcpIn;

        gameLogic.to_lowerTier --> rootObjectUpdater.from_upperTier;
        gameLogic.from_lowerTier <-- rootObjectUpdater.to_upperTier;
        rootObjectUpdater.udpOut --> udp.appIn++;
        udp.appOut++ --> rootObjectUpdater.udpIn;
        rootObjectUpdater.tcpOut --> tcp.appIn++;
        tcp.appOut++ --> rootObjectUpdater.tcpIn;

        rootObjectStore.overlayOut --> overlayStorage.from_upperTier;
        rootObjectStore.overlayIn <-- overlayStorage.to_upperTier;
        rootObjectUpdater.to_lowerTier --> rootObjectStore.from_upperTier;
        rootObjectUpdater.from_lowerTier <-- rootObjectStore.to_upperTier;
        rootObjectStore.udpOut --> udp.appIn++;
        udp.appOut++ --> rootObjectStore.udpIn;
        rootObjectStore.tcpOut --> tcp.appIn++;
        tcp.appOut++ --> rootObjectStore.tcpIn;

        overlayStorage.to_lowerTier --> overlay.appIn;
        overlayStorage.from_lowerTier <-- overlay.appOut;
        overlayStorage.udpOut --> udp.appIn++;
        udp.appOut++ --> overlayStorage.udpIn;
        overlayStorage.tcpOut --> tcp.appIn++;
        tcp.appOut++ --> overlayStorage.tcpIn;

        overlay.udpOut --> udp.appIn++;
        overlay.udpIn <-- udp.appOut++;
        overlay.tcpOut --> tcp.appIn++;
        overlay.tcpIn <-- tcp.appOut++;

        rootObjectStore.to_lowerTier --> updateLayerIM.from_upperTier;
        rootObjectStore.from_lowerTier <-- updateLayerIM.to_upperTier;
        updateLayerIM.udpOut --> udp.appIn++;
        udp.appOut++ --> updateLayerIM.udpIn;
        updateLayerIM.tcpOut --> tcp.appIn++;
        tcp.appOut++ --> updateLayerIM.tcpIn;

        updateLayerIM.to_lowerTier --> updateDissemination.from_upperTier;
        updateLayerIM.from_lowerTier <-- updateDissemination.to_upperTier;
        updateDissemination.udpOut --> udp.appIn++;
        udp.appOut++ --> updateDissemination.udpIn;
        updateDissemination.tcpOut --> tcp.appIn++;
        tcp.appOut++ --> updateDissemination.tcpIn;

        updateDissemination.to_lowerTier --> objectMerger.from_upperTier;
        updateDissemination.from_lowerTier <-- objectMerger.to_upperTier;
        objectMerger.udpOut --> udp.appIn++;
        udp.appOut++ --> objectMerger.udpIn;
        objectMerger.tcpOut --> tcp.appIn++;
        tcp.appOut++ --> objectMerger.tcpIn;

        objectMerger.to_lowerTier --> localObjectUpdater.from_upperTier;
        objectMerger.from_lowerTier <-- localObjectUpdater.to_upperTier;
        localObjectUpdater.udpOut --> udp.appIn++;
        udp.appOut++ --> localObjectUpdater.udpIn;
        localObjectUpdater.tcpOut --> tcp.appIn++;
        tcp.appOut++ --> localObjectUpdater.tcpIn;

        localObjectUpdater.to_lowerTier --> localObjectStore.from_upperTier;
        localObjectUpdater.from_lowerTier <-- localObjectStore.to_upperTier;
        localObjectStore.udpOut --> udp.appIn++;
        udp.appOut++ --> localObjectStore.udpIn;
        localObjectStore.tcpOut --> tcp.appIn++;
        tcp.appOut++ --> localObjectStore.tcpIn;

        localObjectStore.to_lowerTier --> displayUpdater.from_upperTier;
        localObjectStore.from_lowerTier <-- displayUpdater.to_upperTier;
        displayUpdater.udpOut --> udp.appIn++;
        udp.appOut++ --> displayUpdater.udpIn;
        displayUpdater.tcpOut --> tcp.appIn++;
        tcp.appOut++ --> displayUpdater.tcpIn;

        bootstrapList.udpOut --> udp.appIn++;
        bootstrapList.udpIn <-- udp.appOut++;
}

