#include "NTUSignalMultiplexer.h"
#include "NTUListToolManager.h"

NTUSignalMultiplexer::NTUSignalMultiplexer(QObject* parent) : QObject(parent) {}

void NTUSignalMultiplexer::connect(QObject* sender, const char* signal, const char* slot)
{
  Connection conn;
  conn.sender = sender;
  conn.signal = signal;
  conn.slot = slot;

  connections << conn;
  connect(conn);
}

void NTUSignalMultiplexer::connect(const char* signal, QObject* receiver, const char* slot)
{
  Connection conn;
  conn.receiver = receiver;
  conn.signal = signal;
  conn.slot = slot;

  connections << conn;
  connect(conn);
}

bool NTUSignalMultiplexer::disconnect(QObject* sender, const char* signal, const char* slot)
{
  QMutableListIterator<Connection> it(connections);
  while (it.hasNext())
  {
    Connection conn = it.next();
    if ((QObject*)conn.sender == sender && qstrcmp(conn.signal, signal) == 0 && qstrcmp(conn.slot, slot) == 0)
    {
      disconnect(conn);
      it.remove();
      return true;
    }
  }
  return false;
}

bool NTUSignalMultiplexer::disconnect(const char* signal, QObject* receiver, const char* slot)
{
  QMutableListIterator<Connection> it(connections);
  while (it.hasNext())
  {
    Connection conn = it.next();
    if ((QObject*)conn.receiver == receiver && qstrcmp(conn.signal, signal) == 0 && qstrcmp(conn.slot, slot) == 0)
    {
      disconnect(conn);
      it.remove();
      return true;
    }
  }
  return false;
}

void NTUSignalMultiplexer::connect(const Connection& conn)
{
  if (!object)
    return;
  if (!conn.sender && !conn.receiver)
    return;

  if (conn.sender)
    QObject::connect((QObject*)conn.sender, conn.signal, (QObject*)object, conn.slot);
  else
    QObject::connect((QObject*)object, conn.signal, (QObject*)conn.receiver, conn.slot);
}

void NTUSignalMultiplexer::disconnect(const Connection& conn)
{
  if (!object)
    return;
  if (!conn.sender && !conn.receiver)
    return;

  if (conn.sender)
    QObject::disconnect((QObject*)conn.sender, conn.signal, (QObject*)object, conn.slot);
  else
    QObject::disconnect((QObject*)object, conn.signal, (QObject*)conn.receiver, conn.slot);
}

void NTUSignalMultiplexer::setCurrentObject(QObject* newObject)
{
  if (newObject == object)
    return;

  QList<Connection>::ConstIterator it;
  for (it = connections.begin(); it != connections.end(); ++it)
    disconnect(*it);
  object = newObject;
  for (it = connections.begin(); it != connections.end(); ++it)
    connect(*it);

  NTUListToolManager* model = dynamic_cast<NTUListToolManager*>(newObject);
//  if (model)
//    model->emitAllStateSignals();

  // let the world know about who's on top now
  Q_EMIT currentObjectChanged(object);
}
