//
// Created by prova on 04.01.18.
//

#include "LRAModel.h"

int
LRAModel::addVar(LVRef v)
{
    if (has_model.has(v))
        return n_vars_with_model;

    while (int_model.size() <= lva[v].ID()) {
        int_model.push();
        int_lbounds.push();
        int_ubounds.push();
    }
    has_model.insert(v, true);
    write(v, Delta());
    int_lbounds[lva[v].ID()].push({ bs.minusInf(), 0 });
    int_ubounds[lva[v].ID()].push({ bs.plusInf(), 0 });
    return ++n_vars_with_model;
}

void
LRAModel::write(const LVRef &v, const Delta& val)
{
    if ((int_model[lva[v].ID()].size() == 0) || (int_model[lva[v].ID()].last().dl != backtrackLevel())) {
        int_model[lva[v].ID()].push();
        model_trace.push(v);
    }
    ModelEl& el = int_model[lva[v].ID()].last();
    el.d  = val;
    el.dl = backtrackLevel();
}

void
LRAModel::pushBound(const LABoundRef br) {
    LABound& b = bs[br];
    LVRef vr = b.getLVRef();
    if (b.getType() == bound_u) {
        int_ubounds[lva[vr].ID()].push({br, backtrackLevel()});
    }
    else
        int_lbounds[lva[vr].ID()].push({br, backtrackLevel()});

    bound_trace.push(br);
}

void
LRAModel::popBounds()
{
    for (int i = bound_trace.size()-1; i >= limits.last().bound_lim; i--) {
        LABoundRef br = bound_trace[i];
        LABound &b = bs[br];
        LVRef vr = b.getLVRef();
        LABoundRef latest_bound = LABoundRef_Undef;
        if (b.getType() == bound_u) {
            int_ubounds[lva[vr].ID()].pop();
        } else {
            int_lbounds[lva[vr].ID()].pop();
        }
    }
    bound_trace.shrink(bound_trace.size() - limits.last().bound_lim);
}

void
LRAModel::popModels()
{
    assert(limits.size() > 0);
    for (int i = model_trace.size()-1; i >= limits.last().model_lim; i--)
        int_model[lva[model_trace[i]].ID()].pop();
    model_trace.shrink(model_trace.size() - limits.last().model_lim);
}


void LRAModel::printModelState()
{
    printf("We have %d backtrack points\n", getBacktrackSize());
    vec<LVRef> vars;
    has_model.getKeys(vars);
    for (int i = 0; i < vars.size(); i++) {
        LVRef v = vars[i];
        if (has_model[v]) {
            int id = lva[v].ID();
            vec<ModelEl> &vals = int_model[id];
            printf("Var %s has %d models\n", lva.printVar(v), vals.size());
            char *buf = (char*) malloc(1);
            buf[0] = '\0';
            for (int j = 0; j < vals.size(); j++) {
                char *tmp_buf;
                asprintf(&tmp_buf, "%s(%s, %d) ", buf, vals[j].d.printValue(), vals[j].dl);
                free(buf);
                buf = tmp_buf;
            }
            printf(" - %s\n", buf);
            free(buf);
        }
        else
            printf("Var %s has no models\n", lva.printVar(v));
    }
    printf("There are %d bounds in the bound trace\n", bound_trace.size());
    for (int i = 0; i < bound_trace.size(); i++) {
        LABoundRef br = bound_trace[i];
        char* str = bs.printBound(br);
        printf(" - %s\n", str);
        free(str);
    }
}
