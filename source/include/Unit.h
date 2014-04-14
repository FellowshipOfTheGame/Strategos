/*
 *  Strategos - Unit.h
 *
 *  Esta classe eh apresenta um aglomerado de naves, a unidade de batalha
 *  do jogo. (@see Ship.h)
 *  Cada unidade tem um conjunto de taticas associadas que geraram acoes
 *	no campo de batalha. (@see Tactic.h)
 *
 *	Last update: 06/07/2012
 */


#ifndef _UNIT_H_
#define _UNIT_H_

#include <vector>
#include <list>

#include "Ship.h"
#include "Tactic.h"
#include "Action.h"
#include "Dictionary.h"
#include "Image.h"

using namespace std;

void print_MaxActions();

class Unit
{
    private:
        // Imutable Data
        unsigned long id; // TODO: O que exatamente eh id? Posicao na Army? Apenas um numero id usado para taticas?
        const DictKey *mySquadInfo;
        Coordinates bluePrintCoord;
        vector<Tactic*> tactics;

        // Dynamic Combat Data
        int shipsAlive;
        int target;
        Coordinates baseCoord, averageCoord;
        vector<Ship*> ships;
        list<Action*> shipsActions;

        // Taticas basicas
        MoveRandomly basicTacticMoveRandom;
        AttackNearestEnemy basicTacticAttackNearest;
        Retreat basicTacticRetreat;

    public:
        Unit(const Unit* copy);
        Unit(unsigned long ID, const DictKey *info, Coordinates position);
        ~Unit();

        /// Atualiza ships
        int update();

        /// Atualiza Ships e efetua Actions da unidade
        /// \return Retorna o numero de naves vivas
        ///
        /// Esta funcao deve ser chamada apenas uma vez por iteracao da simulacao.
        /// Atualiza e efetua actions calculadas para esta unidade, mais do que uma
        /// chamada poderia dar vantagem para esta unidade.
        /// Atualiza posicao media e numero de naves vivas.
        ///
        void updateActions();

        void addTactic(Tactic *tactic);
        void removeTactic(int n);
        void setTacticAt(Tactic *tactic, int pos);

        // Gets
        Tactic* getTacticAt(unsigned int pos);
        unsigned int getTacticSize();
        unsigned long getID();
        void setID(int id);
        int getType();
        Ship* getShip(unsigned long gid);
        unsigned long nShips() const;
        void setTarget(int i);
        int getTarget();

        /// Recria todas as naves da unidade
        /// Altera a posicao base
        void restoreShips(const Coordinates atBaseP);

        /// Reseta as naves na posicao da BluePrint
        void restoreShips();

        /// Retorna o numero de naves vivas
        ///
        /// \return Retorna o numero de naves vivas
        ///
        /// Este valor eh atualizado na funcao update()
        ///
        int getNShipsAlive();

        /// Verifica se o mouse esta sobre a unidade
        ///
        /// \param  camOX   Posicao X da camera
        /// \param  camOY   Posicao Y da camera
        /// \return Retorna se o mouse esta sobre a nave
        ///
        /// A posicao da camera eh considerada para verificar se o mouse
        /// esta sobre a camera. Eh usada a posicao MEDIA da unidade.
        ///
        bool hover(float camOX, float camOY);

        /// Retorna a posicao media do esquadrao
        ///
        /// \return Retorna a posicao media
        ///
        /// A posicao media eh calculada considerando apenas as naves vivas.
        /// Este valor eh atualizado na funcao update()
        ///
        const Coordinates& getAveragePos() const;

        /// Retorna a coordernada X base INICIAL da batalha
        /// \return Retorna a posicao X base inicial
        float getBaseX();

        /// Retorna a coordernada Y base INICIAL da batalha
        /// \return Retorna a posicao Y base inicial
        float getBaseY();

        /// Retorna a coordernada X media
        float getAvgX();

        /// Retorna a coordernada Y media
        float getAvgY();

        float getBluePrintX();
        float getBluePrintY();
        const Coordinates& getBluePrintCoord();
        void setBluePrintCoord(const Coordinates& coord);

        /// Retorna as coordenadas base INICIAIS
        /// \return Retorna as coordenadas base iniciais da unidade
        const Coordinates& getBaseCoord();

        const shipBaseStats& getSquadBaseStats();
        const DictKey* getUnitInfo() const;

        // Mover Unidade
        void moveTo(Coordinates c);

        // Others
        void printInfo();

        /// Gera acoes para esta unidade
        ///
        /// \param enemyUnits Vetor com unidades INIMIGAS.
        /// \param alliedUnits Vetor com unidades ALIADAS [esta unidade inclusive]
        ///
        void generateActions(TacticValidationData& tvd);

        /// Desenha a unidade considerando a posicao da camera
        void render();
//        int amountAlive();
};

#endif
