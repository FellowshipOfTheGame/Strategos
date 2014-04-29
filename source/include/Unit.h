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


#include "CombatLog.h"
#include "Ship.h"
#include "Tactic.h"
#include "Action.h"
#include "Dictionary.h"
#include "Image.h"

void print_MaxActions();

class Unit
{
    private:
        // Imutable Data
        Coordinates bluePrintCoord;
        const DictKey *mySquadInfo;
        std::vector<Tactic*> tactics;

        // Team Data
        unsigned int id;   // TODO: O que exatamente eh id? Posicao na Army? Apenas um numero id usado para taticas?
        int team;  // Time 0 ou 1, durante a batalha.

        // Dynamic Combat Data
        int shipsAlive;
        Unit* target;
        Coordinates baseCoord, averageCoord;
        std::vector<Ship*> ships;
        std::list<Action*> shipsActions;

        // Taticas basicas
        MoveRandomly basicTacticMoveRandom;
        AttackNearestEnemy basicTacticAttackNearest;
        Retreat basicTacticRetreat;

    public:
        Unit(const Unit* copy);
        Unit(unsigned int ID, const DictKey *info, Coordinates position);
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

        // Taticas
        void addTactic(Tactic *tactic);
        void removeTactic(int n);
        void setTacticAt(Tactic *tactic, int pos);
        Tactic* getTacticAt(unsigned int pos);
        unsigned int getTacticSize();

        // Team
        unsigned int getID() const;
        void setID(int id);
        int getTeam() const;

        /// Retorna o tipo da unidade
        int getType() const;

        void setTarget(Unit* unit);
        Unit* getTarget() const;

        /// Ships
        Ship* getShip(unsigned long gid);
        unsigned long nShips() const;

        /// Recria todas as naves da unidade
        /// Altera a posicao base
        void restoreUnit(int teamID, const Coordinates atBaseP);

        /// Reseta as naves na posicao da BluePrint
        void restoreUnit(int teamID);

        /// Retorna o numero de naves vivas
        ///
        /// \return Retorna o numero de naves vivas
        ///
        /// Este valor eh atualizado na funcao update()
        ///
        int getNShipsAlive() const;

        /// Verifica a posicao esta sobre a unidade
        ///
        /// \param  mX   Posicao X
        /// \param  mY   Posicao Y
        /// \return Retorna se a posicao X,Y esta sobre a unidade
        ///
        /// A posicao MEDIA da unidade eh usada.
        ///
        bool hover(float mX, float mY) const;

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
        float getBaseX() const;

        /// Retorna a coordernada Y base INICIAL da batalha
        /// \return Retorna a posicao Y base inicial
        float getBaseY() const;

        /// Retorna a coordernada X media
        float getAvgX() const;

        /// Retorna a coordernada Y media
        float getAvgY() const;

        float getBluePrintX() const;
        float getBluePrintY() const;
        const Coordinates& getBluePrintCoord() const;
        void setBluePrintCoord(const Coordinates& coord);

        /// Retorna as coordenadas base INICIAIS
        /// \return Retorna as coordenadas base iniciais da unidade
        const Coordinates& getBaseCoord() const;

        const shipBaseStats& getSquadBaseStats() const;
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

        CombatRound* unifyCombatRound();
};

#endif
