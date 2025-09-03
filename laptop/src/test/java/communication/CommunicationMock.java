package communication;

import model.Data;

class CommunicationMock extends Communication {
    private Data receivedData;

    public CommunicationMock() {

    }

    @Override
    public synchronized void onPacketReceived(int seq, Data data) {
        this.receivedData = data;
    }

    public Data getReceivedData() {
        return receivedData;
    }
}
