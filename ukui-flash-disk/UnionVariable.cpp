#include "UnionVariable.h"

static QList<std::shared_ptr<Peony::Mount>> mountsList;

QList<std::shared_ptr<Peony::Mount> > *findList()
{
    return &mountsList;
}
